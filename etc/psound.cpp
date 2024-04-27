#include <iostream>
#include <vector>
#include <cstring>
#include <portaudio.h>

// サンプリング周波数
#define SAMPLE_RATE 44100
// サンプルのバッファサイズ
#define FRAMES_PER_BUFFER 256

// コールバック関数
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    // マイクからの入力を直接出力にコピーする
    memcpy(outputBuffer, inputBuffer, framesPerBuffer * sizeof(float));
    return paContinue;
}

int main()
{
    PaError err;
    PaStream *stream;

    // PortAudioの初期化
    err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // 入力デバイス（マイク）の設定
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice(); // デフォルトの入力デバイスを使用
    inputParameters.channelCount = 1;                    // モノラル
    inputParameters.sampleFormat = paFloat32;            // float型のサンプル
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    // 出力デバイス（スピーカー）の設定
    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice(); // デフォルトの出力デバイスを使用
    outputParameters.channelCount = 1;                     // モノラル
    outputParameters.sampleFormat = paFloat32;             // float型のサンプル
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    // ストリームを開く
    err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLE_RATE,
                        FRAMES_PER_BUFFER, paClipOff, paCallback, nullptr);
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return 1;
    }

    // ストリームを開始
    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    std::cout << "Recording and playing. Press Ctrl+C to stop." << std::endl;

    // Ctrl+Cが押されるまで待機
    while (true)
    {
        Pa_Sleep(1000);
    }

    // ストリームを停止
    err = Pa_StopStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }

    // ストリームを閉じる
    err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }

    // PortAudioの終了
    Pa_Terminate();

    return 0;
}
