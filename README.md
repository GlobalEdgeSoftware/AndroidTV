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

