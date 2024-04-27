/*
* SSMから音声データを取得し、再生するプログラム
* author Shunsuke.I
* date : 2024/04/28
*/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ssm.hpp>
#include <pulse/error.h>  /* pulseaudio */
#include <pulse/simple.h> /* pulseaudio */
#include "record.hpp"

static void ctrlC(int aStatus);
static void setSigInt();
static void Terminate(void);
static void setupSSM(void);

static int gShutOff = 0;
static unsigned int dT = 10; // 10ms

static SSMApi<rec_sound, rec_sound_property> *PLAYBACK;

int main()
{
    SSMApi<rec_sound, rec_sound_property> pymusic(SOUNDHANDLER_SNAME, 0);
    PLAYBACK = &pymusic;
    rec_sound *pydata = &pymusic.data;

    pa_simple *playback_stream;
    pa_sample_spec playback_spec;
    float buf[BUF_SIZE / SAMPLE_SIZE];
    int pa_errno, pa_result, written_bytes;

    // 再生用のパラメータ使用
    playback_spec.format = PA_SAMPLE_FLOAT32LE;
    playback_spec.rate = SAMPLE_RATE;
    playback_spec.channels = CHANNELS;

    playback_stream = pa_simple_new(NULL, "playback", PA_STREAM_PLAYBACK, NULL, "playback", &playback_spec, NULL, NULL, &pa_errno);
    if (playback_stream == NULL)
    {
        fprintf(stderr, "ERROR: Failed to connect pulseaudio server: %s\n", pa_strerror(pa_errno));
        return EXIT_FAILURE;
    }
    try
    {
        setupSSM();
        setSigInt();
        bool update[1] = {false};
        SSM_tid update_id[1] = {-1};
#define INDEX_MUSIC 0
        while (!gShutOff)
        {
            update[INDEX_MUSIC] = false;
            if (update_id[INDEX_MUSIC] < getTID_top(pymusic.getSSMId()))
            {
                pymusic.readLast();
                update[INDEX_MUSIC] = true; // 最新情報を読み込む
                update_id[INDEX_MUSIC] = pymusic.timeId;
            }
            else
            {
                update[INDEX_MUSIC] = false;
            }
            if (update[INDEX_MUSIC])
            {
                pa_result = pa_simple_write(playback_stream, pydata->buf, sizeof(pydata->buf), &pa_errno);
                if (pa_result < 0)
                {
                    fprintf(stderr, "ERROR: Failed to write data to pulseaudio: %s\n", pa_strerror(pa_errno));
                    return EXIT_FAILURE;
                }
            }
            usleep(dT * 1000);
        }
    }
    catch (std::runtime_error const &error)
    {
        std::cout << error.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "An unknown fatal error has occured. Aborting." << std::endl;
    }

    pa_simple_free(playback_stream);
    Terminate();
    return EXIT_SUCCESS;
}
static void ctrlC(int aStatus)
{
    signal(SIGINT, NULL);
    gShutOff = true;
}
static void setSigInt()
{
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = ctrlC;
    sigaction(SIGINT, &sig, NULL);
}
static void Terminate(void)
{
    PLAYBACK->release();
    endSSM();
    printf("\nend\n");
}
static void setupSSM(void)
{
    std::cerr << "initializing ssm ... ";
    if (!initSSM())
        throw std::runtime_error("[\033[1m\033[31mERROR\033[30m\033[0m]:fail to initialize ssm.");
    else
        std::cerr << "OK.\n";

    //
    std::cerr << "open sound ... ";
    if (!PLAYBACK->open(SSM_READ))
    {
        throw std::runtime_error("[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open sound on ssm.\n");
    }
    else
    {
        std::cerr << "OK.\n";
    }
}