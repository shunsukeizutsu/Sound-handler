マイクからの音声データをSSMへ送るプログラム

SSMから取得した音声データを再生プログラム

実行方法
1. mkdir bin
2. cd bin
3. cmake ..
4. make
5. ./record   ==>SSMに音声データを送る
6. ./playback ==>SSMから音声データを取得して再生する