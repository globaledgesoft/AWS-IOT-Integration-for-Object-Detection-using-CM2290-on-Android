

Description: 
	This is based application which is able to detect the 91 types of different object from device camera
	also it is able to count the number of object in frame and send the data to aws iot cloud,
	data (object name + count of object).

Requirement:
	1.Android Studio based:
		https://developer.android.com/studio?gclsrc=aw.ds 

	2.Tool chain SDK. 
		https://www.thundercomm.com/product/cm2290-c2290-development-kit/
	
	3.AWS Account For user pool and identity pool) 
	    Create a user pool(Setting):
		setup user pool for secure login using Cognito follow the steps below:
		
		 * Go to https://console.aws.amazon.com/cognito/
		 * Click `Manage User Pools`
		 * Click `Create a user pool`
		 * Fill-in `Pool name`
		 * Click `Review defaults`
		 * Click `Create user pool`
		 * Copy `Pool Id`
		 * Select `App clients` in the left navigation.
		 * Click `Add an app client`
		 * Fill-in `App client name`
		 * Click `Create app client`
		 * Click `Show details` and copy `App client id` and `App client secret`  


	4.Include AWS ioT device  SDK for android: 
		Inside gradel.build file ,dependencies section add line: 
		
			implementation 'com.amazonaws:aws-android-sdk-iot:2.49.0'
			implementation 'com.google.android.material:material:1.4.0' 
			implementation 'androidx.constraintlayout:constraintlayout:2.1.2' 
			testImplementation 'org.junit.jupiter:junit-jupiter' 
			androidTestImplementation 'androidx.test.ext:junit:1.1.3' 
			androidTestImplementation 'androidx.test.espresso:espresso-core:3.4.0' 

	5.Setup the SNPE SDK in the host system:
		https://developer.qualcomm.com/software/qualcomm-neural-processing-sdk

After all dependencied added ,Run app:
	1.Open the app build the project
	2.After completing build step run the app on selected device.

