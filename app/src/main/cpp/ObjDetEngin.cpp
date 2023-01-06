#include "ObjDetEngin.h"
#include "SnpeC.hpp"
#include "CameraNatives.h"
#include <unistd.h>
#include <vector>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <string>
#include <cstdlib>
#include <mutex>
#include <glob.h>
#include <dirent.h>
#include <stdarg.h>
#include<jni.h>
#include<thread>


ObjDetEngin::ObjDetEngin(): camera_ready(false), image_m(nullptr), image_reader_p(nullptr), native_cam(nullptr){}

ObjDetEngin::~ObjDetEngin(){
    JNIEnv *env;
    java_vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    env->DeleteGlobalRef(calling_activity_obj);
    calling_activity_obj = nullptr;

    // Camera_capturing_session;
    if (native_cam != nullptr) {
        delete native_cam;
        native_cam = nullptr;
    }

    //check that we don't leak windows native
    if (m_native_window != nullptr) {
        ANativeWindow_release(m_native_window);
        m_native_window = nullptr;
    }

    if (image_reader_p != nullptr) {
        delete (image_reader_p);
        image_reader_p = nullptr;
    }
}

void ObjDetEngin::OnCreate() {
    snp_obj = new SnpeC(model_path, 2, output_layers);
//    createEmbeddings(gt_img_dir.c_str());
}


void ObjDetEngin::OnPause() {}
void ObjDetEngin::OnDestroy() {}

void ObjDetEngin::SetNativeWindow(ANativeWindow* native_window) {
    // Save native window
    m_native_window = native_window;
}

void ObjDetEngin::SettingCamera() {
    native_cam = new CameraNatives(m_selected_camera_type);
    native_cam->MatchCaptureSizeRequest(&m_view,
                                             ANativeWindow_getWidth(m_native_window),
                                             ANativeWindow_getHeight(m_native_window));
    ASSERT(m_view.width && m_view.height, "Could not find supportable resolution");

    ANativeWindow_setBuffersGeometry(m_native_window, m_view.width, m_view.height,
                                     WINDOW_FORMAT_RGBX_8888);
    image_reader_p = new Image_Reader(&m_view, AIMAGE_FORMAT_YUV_420_888);
    image_reader_p->SetPresentRotation(native_cam->GetOrientation());
    ANativeWindow* image_reader_window = image_reader_p->GetNativeWindow();
    camera_ready = native_cam->CreateCaptureSession(image_reader_window);
}

int i=0;

void ObjDetEngin::loopCam(JNIEnv* env,jobject obj) {
    bool buffer_printout = false;
    bool keepRunning = true;
    myVector.resize(91);

    //below j objects are used to find java method and call it from cpp
    jclass cls = env->FindClass("com/example/f1_object_detection_cm2290/MainActivity");
    jmethodID method = env->GetMethodID(cls, "publish_from_cpp", "([I)V");
    int count =0;
    while (1) {
        count ++;
        if (m_camera_thread_stopped) { break; }
        if (!camera_ready || !image_reader_p) { continue; }
        //reading the image from ndk reader
        image_m = image_reader_p->GetNextImage();
        if (image_m == nullptr) { continue; }

        ANativeWindow_acquire(m_native_window);
        ANativeWindow_Buffer buffer;
        if (ANativeWindow_lock(m_native_window, &buffer, nullptr) < 0) {
            image_reader_p->DeleteImage(image_m);
            image_m = nullptr;
            continue;
        }
        if (false == buffer_printout) {
            buffer_printout = true;
            LOGI("/// H-W-S-F: %d, %d, %d, %d", buffer.height, buffer.width, buffer.stride,
                 buffer.format);
        }
        //display the image
        image_reader_p->DisplayImage(&buffer, image_m);
        img_mat = cv::Mat(buffer.height, buffer.stride, CV_8UC4, buffer.bits);
        bgr_img = cv::Mat(img_mat.rows, img_mat.cols, CV_8UC3);
        cv::cvtColor(img_mat, bgr_img, cv::COLOR_RGBA2BGR);
        cv::Mat res_img = cv::Mat(300, 300, CV_8UC3);
        cv::resize(bgr_img, res_img, cv::Size(300, 300));

        pred_out = snp_obj->obj_predict(res_img);

        found = postprocess(pred_out, img_mat.rows, img_mat.cols);
        int cls_num = 0;

        std::fill(myVector.begin(), myVector.end(), 0);
            for(auto r :found) {
            LOGI("-----------%s------------    %d   --------  %s \n", "into fpr after postprocess", class_indexs[cls_num], class_names[class_indexs[cls_num] - 1].c_str());
            cv::rectangle(img_mat, r, cv::Scalar(0, 0, 255), 2);
            cv::putText(img_mat, class_names[class_indexs[cls_num]-1], r.tl(), cv::FONT_HERSHEY_SIMPLEX ,3,
            cv::Scalar(0, 255, 255), 1);
            myVector[class_indexs[cls_num]-1]++;
            cls_num++;
        }
        class_indexs.clear();
        found.clear();
        pred_out.clear();
        ANativeWindow_unlockAndPost(m_native_window);
        ANativeWindow_release(m_native_window);
    i++;
    //here java publish method get called after every 30 frames
    if(count%30 ==0 ){
        jintArray arr = env->NewIntArray(myVector.size());
        env->SetIntArrayRegion(arr, 0, myVector.size(), myVector.data());
        env->CallVoidMethod(obj,method,arr);
        }
    }
}


std::vector<cv::Rect> ObjDetEngin::postprocess(std::map<std::string, std::vector<float>> out, float video_height, float video_width) {
    float probability;
    int class_index;
    auto &boxes = out[BOXES_TENSOR];
    auto &scores = out[SCORES_TENSOR];
    auto &classes = out[CLASSES_TENSOR];

//    LOGI("%d\n", boxes.size());
    for(size_t cur=0; cur < scores.size(); cur++) {
        probability = scores[cur];
        class_index = static_cast<int>(classes[cur]);
        if(probability < PROBABILITY_THRESHOLD)
            continue;
        LOGI("-----------------%d\n", class_index);
        auto y1 = static_cast<int>(boxes[4 * cur] * video_height);
        auto x1 = static_cast<int>(boxes[4 * cur + 1] * video_width);
        auto y2 = static_cast<int>(boxes[4 * cur + 2] * video_height);
        auto x2 = static_cast<int>(boxes[4 * cur + 3] * video_width);
        found.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
        class_indexs.push_back(class_index);
    }
    return found;
}
