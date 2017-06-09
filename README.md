
# AndroidTV
Android TV bring Up on Dragon Board 410C

Download the latest Source Code for DragonBoard 410C 

Add the below two lines in the device/qcom/msm8916_64.mk file

PRODUCT_CHARACTERISTICS := tv
$(call inherit-product, device/google/atv/products/atv_base.mk)

Build your android 410C source code on Linux Platform .

Download and Build Live Tv app for arm64

tapas LiveTv arm64

make LiveTv

cp -r out/target/product/generic_arm64/system//priv-app/LiveTv/ out/system/priv-app/

rm -rf out/target

cd -

Build the Sample Input Apk and its dependencies (Exoplayer) using the gradle scripts:

mkdir -p apps/

cd apps/

export ANDROID_HOME=<path to sdk>

echo "count=0" > /home/buildslave/.android/repositories.cfg

rm -rf ExoPlayer androidtv-sample-inputs

Building apps and dependencies

git clone https://github.com/googlesamples/androidtv-sample-inputs

cd androidtv-sample-inputs/

sed -i "s/23.0.3/25.0.2/g" app/build.gradle library/build.gradle

./gradlew assembleDebug

cp app/build/outputs/apk/app-debug.apk ../out/target/product/msm8916_64/data/app/

cd -

git clone https://github.com/google/ExoPlayer

cd ExoPlayer

sed -i "s/23.0.3/25.0.2/g" build.gradle

./gradlew assembleDebug

cp ./demo/buildout/outputs/apk/demo-withExtensions-debug.apk ../out/target/product/msm8916_64/data/app/

cd -

Again prepare image .

IR Sensor Driver and Universal Remote Integration:

1)A GPIO based IR Sensor is integrated on to Dragon Board 410C with the     help of Groove Sensor Kit and Sensor Driver is implemented.

2) RC6 Protocol based Universal Remote is integrated with the Android TV  by Registering the driver with Input Subsystem and input events are generated

3) The idc (Input Device Configurstion)Configuration file is generated in order to configure the IR Remote as an Input Device like mouse and keyboard

4) Linux and Android Key Mappings are done by generating the Specific kl(Keylayout) and kcm(Key Character Mapping) files and specific Key codes are generated for each button on IR Remote
For example:
                              
     home button: KEY_HOME
     play button: KEY_PLAY

Custom TV Launcher Creation:

A Customized TV Launcher is created using the Lean back Library. Detailed steps are explained below:

1)	Add the following attribute to activity:
                     android:launchMode="singleTask"
       Then add two categories to the intent filter :
        <category android:name="android.intent.category.DEFAULT" />	   <category android:name="android.intent.category.HOME" />
        
2)	Enable the TV Platform Option while creating application in Android Studio

3)	Make blank activity, named “MainActivity“. 

4)	Android studio now generates 2 files, Java class & layout/activity_main.xml. (we don’t use res/menu/menu_main.xml)
* Note: We can also find “Android TV activity”. When you select this, it will create two match files at the same time. It’s a really helpful reference, but difficult to understand what kind of functions each files takes care of. So I will create these files from scratch in this post so that we can understand each source code’s responsibility. Many of the implementation in this post is referencing this official sample program.
New -> Java Class -> Name: MainFragment

5)	Instead of above procedure, if we choose New -> Fragment -> Blank fragment Uncheck “Create layout XML?”, it makes two match sample source code.

6)	First, modify activity_main.xml as follows so that it only displays main fragment.
activity_main.xml
      <?xml version="1.0" encoding="utf-8"?>
 
      <fragment xmlns:android="http://schemas.android.com/apk/res/android"
 
      xmlns:app="http://schemas.android.com/apk/res-auto"
      xmlns:tools="http://schemas.android.com/tools" android:id="@+id/main_browse_fragment"
 
android:name="com.corochann.androidtvapptutorial.MainFragment"
      android:layout_width="match_parent"
 
      android:layout_height="match_parent" tools:context=".MainActivity" tools:deviceIds="tv"

  7)  Second,  modify MainFragment as follows: 

We will make this MainFragment as a sub-class of BrowseFragment. BrowseFragment class is supplied by Android SDK Leanback library, and it creates standard UI for Android TV application
 
MainActivity.java
@Override
 
public void onActivityCreated(Bundle savedInstanceState) {
 
       Log.i(TAG, "onActivityCreated");
 
       super.onActivityCreated(savedInstanceState);
       setupUIElements();
 
}
 private void setupUIElements() {
 
// setBadgeDrawable(getActivity().getResources().getDrawable(R.drawable.videos_by_google_banner));
 
         setTitle("Hello Android TV!"); // Badge, when set, takes precedent
 
         // over title
 
        setHeadersState(HEADERS_ENABLED);
 
       setHeadersTransitionOnBackEnabled(true); 
 
       // set fastLane (or headers) background color
 
       setBrandColor(getResources().getColor(R.color.fastlane_background));
 
       // set search icon color
 
       setSearchAffordanceColor(getResources().getColor(R.color.search_opaque));
 
}

  8) Use Package Manager to get all the application details which is installed in the device and display them with Event Listener.

References:
1.https://source.android.com/devices/input/input-device-configuration-files
2.https://developer.android.com/reference/android/view/InputDevice.html
3.https://seasonofcode.com/posts/internal-input-event-handling-in-the-linux-kernel-and-the-android-userspace.html
4.http://newandroidbook.com/Book/Input.html?r
5.http://corochann.com/android-tv-application-hands-on-tutorial-1-45.html
6.https://stackoverflow.com/questions/4841686/how-to-make-a-launcher
7.https://developer.android.com/training/tv/index.html
8.https://code.tutsplus.com/tutorials/an-introduction-to-android-tv--cms-23251
 



