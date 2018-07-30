Very fast line count program that matches the performance of wc command

Please note that wc program caches the count after first run. So if you want to compare, always compare with first attempt of wc as subsequent runs are cached results


how to compile

gcc -O3 -o main main.c

time ./main FILENAME


compare with wc like


time wc FILENAME


