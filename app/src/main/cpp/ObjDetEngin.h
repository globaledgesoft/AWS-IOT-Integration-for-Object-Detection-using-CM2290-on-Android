#include <android/asset_manager.h>
#include <android/native_window.h>
#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/features2d.hpp>
#include "Image_Reader.h"
#include "CameraNatives.h"
#include "Util.h"
#include <unistd.h>
#include <time.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <thread>
#include "SnpeC.hpp"
#include "CameraNatives.h"
#include <vector>

#define OUTPUT_LAYER_1 "Postprocessor/BatchMultiClassNonMaxSuppression"
#define OUTPUT_LAYER_2 "add_6"
#define BOXES_TENSOR "Postprocessor/BatchMultiClassNonMaxSuppression_boxes"
#define SCORES_TENSOR "Postprocessor/BatchMultiClassNonMaxSuppression_scores"
#define CLASSES_TENSOR "detection_classes:0"
#define PERSON_CLASS_INDEX 1

#define PROBABILITY_THRESHOLD 0.5

class ObjDetEngin{
public:
    ObjDetEngin();
    ~ObjDetEngin();
    ObjDetEngin(const ObjDetEngin& other) = delete;
    ObjDetEngin& operator=(const ObjDetEngin& other) = delete;

    void OnCreate();
    void OnPause();
    void OnDestroy();
    void SetJavaVM(JavaVM* pjava_vm) { java_vm = pjava_vm; }
    void SetNativeWindow(ANativeWindow* native_indow);
    void SettingCamera();
    void  loopCam(JNIEnv* env,jobject obj);
private:
    JavaVM* java_vm;
    jobject calling_activity_obj;
    ANativeWindow* m_native_window;
    ANativeWindow_Buffer m_native_buffer;
    CameraNatives* native_cam;
    camera_type m_selected_camera_type = BACK_CAMERA; // Default
    ImageFormat m_view{0, 0, 0};
    Image_Reader* image_reader_p;
    AImage* image_m;
    volatile bool camera_ready;
    // for timing OpenCV bottlenecks
    clock_t start_t, end_t;
    // Used to detect up and down motion
    bool scan_mode;

    // OpenCV values
    cv::Mat img_mat;
    cv::Mat bgr_img;
    cv::Mat crop;
    cv::Mat grey_img;
    cv::Mat grey_res_img;
    std::vector<int> class_indexs;
    bool m_camera_thread_stopped = false;
    SnpeC *snp_obj;
    int i;
    std::vector<int> myVector;
    std::string model_path = "/storage/emulated/0/appData/models/mobilenet_ssd_1.dlc";
    std::vector<cv::Rect> found;
    std::vector<int> class_indexes;
    std::vector<std::string> output_layers {OUTPUT_LAYER_1, OUTPUT_LAYER_2};
    std::vector<std::string> class_names {"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "street sign", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "hat", "backpack", "umbrella", "shoe", "eye glasses", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "plate", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "mirror", "dining table", "window", "desk", "toilet", "door", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "blender", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush", "hair brush"};
    std::map<std::string, std::vector<float>> pred_out;
    std::map<std::string, std::vector<float>> gt_faces;
    cv::CascadeClassifier face;

    const int out_model_vec = 512;
    int face_inp_shape = 112;
    std::vector<cv::Rect> postprocess(std::map<std::string, std::vector<float>> out, float video_height, float video_width);
    float find_average(std::vector<float> &vec);

    };

