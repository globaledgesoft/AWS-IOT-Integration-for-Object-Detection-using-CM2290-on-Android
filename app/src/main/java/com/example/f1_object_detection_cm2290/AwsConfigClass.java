package com.example.f1_object_detection_cm2290;

import static com.example.f1_object_detection_cm2290.AwsConfigConstants.COGNITO_POOL_ID;
import static com.example.f1_object_detection_cm2290.AwsConfigConstants.CUSTOMER_SPECIFIC_ENDPOINT;
import static com.example.f1_object_detection_cm2290.AwsConfigConstants.MY_REGION;
import android.content.Context;
import android.util.Log;
import com.amazonaws.auth.CognitoCachingCredentialsProvider;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttClientStatusCallback;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttManager;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttNewMessageCallback;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttQos;
import java.io.UnsupportedEncodingException;
import java.util.UUID;

public class AwsConfigClass {
    public static final String TAG = AwsConfigClass.class.getSimpleName();

    AWSIotMqttManager mqttManager;
    String clientId = UUID.randomUUID().toString();

    CognitoCachingCredentialsProvider credentialsProvider;

    public void startAwsConfigurations(Context context) {

        credentialsProvider = new CognitoCachingCredentialsProvider(
                context,
                COGNITO_POOL_ID,
                MY_REGION
        );

        mqttManager = new AWSIotMqttManager(clientId, CUSTOMER_SPECIFIC_ENDPOINT);
        mqttManager.setMaxAutoReconnectAttempts(5);

        mqttManager.setKeepAlive(10);

        try {
            mqttManager.connect(credentialsProvider, new AWSIotMqttClientStatusCallback() {
                @Override
                public void onStatusChanged(AWSIotMqttClientStatus status, Throwable throwable) {
                    Log.d(TAG, "Status = " + status);

                    if (status == AWSIotMqttClientStatus.Connecting) {
                        Log.d(TAG, "Connecting...");
                    } else if (status == AWSIotMqttClientStatus.Connected) {
                        Log.d(TAG, "Connected");
//
//                        Log.d(TAG, "Subscribed on : sdk/test/Python");

                    } else if (status == AWSIotMqttClientStatus.Reconnecting) {
                        if (throwable != null) {
                            Log.d(TAG, "Connection error.", throwable);
                        }
                        Log.d(TAG, "Reconnecting");
                    } else if (status == AWSIotMqttClientStatus.ConnectionLost) {
                        if (throwable != null) {
                            Log.e(TAG, "Connection error.", throwable);

                        }
                        Log.d(TAG, "Disconnected");
                    } else {
                        Log.d(TAG, "Disconnected");

                    }
                }
            });
        } catch (final Exception e) {
            Log.e(TAG, "Connection error...", e);
        }

    }

    public void subscribeToTopic(String topic){
        try {
            mqttManager.subscribeToTopic(topic, AWSIotMqttQos.QOS0,
                    new AWSIotMqttNewMessageCallback() {
                        @Override
                        public void onMessageArrived(final String topic, final byte[] data) {
                            try {
                                String message = new String(data, "UTF-8");
                                Log.d(TAG, "Message arrived:");
                                Log.d(TAG, "   Topic: " + topic);
                                Log.d(TAG, " Message: " + message);

                            } catch (UnsupportedEncodingException e) {
                                Log.e(TAG, "Message encoding error.", e);
                            }
                        }

                    });
        } catch (Exception e) {
            Log.e(TAG, "Subscription error.", e);
        }

    }

    public void publishData(String msg, String topic){
        try {
            mqttManager.publishString(msg, topic, AWSIotMqttQos.QOS1);
        } catch (Exception e) {
            Log.e(TAG, "Publish error.", e);
        }
    }
}



