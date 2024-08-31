param($p1)

if(Test-Path "./ndkpath.txt") {
    ndkpath = Get-Content "./ndkpath.txt"
}
else
{
    $ndkpath = $env:ANDROID_NDK_HOME
}
$child = Get-ChildItem -Path $ndkpath/toolchains/llvm/prebuilt -Name
& $ndkpath/toolchains/llvm/prebuilt/$child/bin/llvm-addr2line -e ./build/debug/libcinema.so $p1