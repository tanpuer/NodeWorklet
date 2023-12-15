#sh
# If have any problem check the document: https://yuque.antfin-inc.com/agil/skill/ttp2p7
# ========== start default vscode project ===========
# sh ./compile.sh
# ========== start xcode project ===========
# sh ./compile.sh xode

XCODE=""
DIR=agil
OUTPUT=./agil
MAKE='make -j16'

echo "======= start to gen the $DIR project======="
rm -rf $DIR
mkdir -p $DIR
mkdir -p $OUTPUT
cd $DIR
cmake ../../ -DCMAKE_BUILD_TYPE=Debug -DHERON_BUILD_MACOS=true -DHERON_MINI_PROGRAM=true -DHERON_REACT=true -DHERON_WEEX=false -DAGIL_NS_DEBUG=true -DAGILREACT_BUILD_MACOS=ON -DAGILREACT_BUILD_NODEJS=true $XCODE
$MAKE
cd ..
echo "======= finished gen the $DIR project ======="

# copy the GLES libs to nodejs dir
#get_arch=$(uname -a)
#if [[ $get_arch =~ "x86_64" ]];then
#    cp -rf ../../../agil/agil4xplatform/third_party/angle/lib/mac/x86/* ./
#elif [[ $get_arch =~ "arm64" ]];then
#    cp -rf ../../../agil/agil4xplatform/third_party/angle/lib/mac/arm64/* ./
#else
#    echo "other arch"
#fi