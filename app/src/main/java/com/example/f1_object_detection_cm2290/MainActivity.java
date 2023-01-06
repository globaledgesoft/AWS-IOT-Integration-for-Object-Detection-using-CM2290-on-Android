package com.example.f1_object_detection_cm2290;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.IntStream;
import android.app.Activity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.util.Log;
import android.widget.PopupWindow;
import com.google.gson.Gson;
import com.example.awsmqtt.R;
public class MainActivity extends AppCompatActivity {
    volatile MainActivity _savedInstance;
    PopupWindow _popupwindow;
    // Used to load the 'onetry' library on application startup.
    static {
        System.loadLibrary("f1_object_detection_cm2290");
    }
    static final String TAG = "F1_object_detection_cm2290";

    private static final int PERMISSION_REQUEST_CODE_CAMERA = 1;
    SurfaceView msurfaceview;
    SurfaceHolder msurfaceholder;
    AwsConfigClass awsConfigClass = new AwsConfigClass();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        awsConfigClass.startAwsConfigurations(this);
        try {
            Thread.sleep(8000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        super.onCreate(savedInstanceState);
        _savedInstance = this;
//        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);

        String[] accessPermissions = new String[]{Manifest.permission.CAMERA, Manifest.permission.READ_EXTERNAL_STORAGE};

        boolean prereq = false;
        for (String access : accessPermissions) {
            int curperm = ActivityCompat.checkSelfPermission(this, access);
            if (curperm != PackageManager.PERMISSION_GRANTED) {
                prereq = true;
                break;
            }
        }
        if (prereq) {
            ActivityCompat.requestPermissions(this, accessPermissions, PERMISSION_REQUEST_CODE_CAMERA);
        }

        onCreateJNI(this, getAssets());
        msurfaceview = (SurfaceView) findViewById(R.id.surfaceView);
        msurfaceholder = msurfaceview.getHolder();
        msurfaceholder.addCallback(new SurfaceHolder.Callback() {
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.v(TAG, "surfaceChanged format=" + format + ", width=" + width + ", height=" + height);
            }

            public void surfaceCreated(SurfaceHolder holder) {
                Log.v(TAG, "surfaceCreated");
                setSurface(holder.getSurface());
            }

            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.v(TAG, "surfaceDestroyed");
            }
        });
        CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        try {
            for (String cameraId : manager.getCameraIdList()) {
                CameraCharacteristics characteristics
                        = manager.getCameraCharacteristics(cameraId);

                Log.d("Img", "INFO_SUPPORTED_HARDWARE_LEVEL " + characteristics.get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL));
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL FULL" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_FULL);
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL 3" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_3);
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL LIMITED" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED);
                Log.d("Img", "INFO_REQUIRED_HARDWARE_LEVEL LEGACY" + CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY);

            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    //91 classed string array
    String[] classes = {"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "street sign", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "hat", "backpack", "umbrella", "shoe", "eye glasses", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "plate", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "mirror", "dining table", "window", "desk", "toilet", "door", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "blender", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush", "hair brush"};

    //this method is specially to send data to AWS IOT cloud , it get called from CPP code
    //after every 30 frame this method publish  the object name and count  to AWS cloud
    public void publish_from_cpp(int arr[]) {
        Map<String, Object> map = new HashMap<>();
        IntStream.range(0, arr.length).boxed().forEach(i -> map.put(classes[i], arr[i]));
        String jsonData = new Gson().toJson(map);
        System.out.println("================" + jsonData);
        awsConfigClass.publishData(jsonData, "my/obj_detection");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    /**
     * A native method that is implemented by the 'onetry' native library,
     * which is packaged with this application.
     */
    public native void onCreateJNI(Activity callerActivity, AssetManager assetManager);

    public native void setSurface(Surface surface);
}



