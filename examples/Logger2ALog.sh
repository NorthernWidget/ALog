find -name "*.ino" | xargs sed -i 's/logger\./alog\./g'
find -name "*.ino" | xargs sed -i 's/Logger\ logger/ALog\ alog/g'
find -name "*.ino" | xargs sed -i 's/include\ \"Logger\.h\"/include\ \"ALog\.h\"/g'
