#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ssm.hpp>
#include <pulse/error.h>  /* pulseaudio */
#include <pulse/simple.h> /* pulseaudio */
#include "recordmusic.hpp"

#define APP_NAME "pulseaudio_sample"
#define STREAM_NAME "play"
#define DATA_SIZE 1024

static void ctrlC(int aStatus);
static void setSigInt();
static void Terminate(void);
static void setupSSM(void);

static int gShutOff = 0;
static unsigned int dT = 10; // 10ms

static SSMApi<rec_music, rec_music_property> *PYMUSIC;

int main()
{
    SSMApi<rec_music, rec_music_property> pymusic(RECORDMUSIC_SNAME, 0);
    PYMUSIC = &pymusic;
    rec_music *pydata = &pymusic.data;

    int pa_errno, pa_result, read_bytes;

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = 48000;
    ss.channels = 1;

    pa_simple *pa = pa_simple_new(NULL, APP_NAME, PA_STREAM_PLAYBACK, NULL, STREAM_NAME, &ss, NULL, NULL, &pa_errno);
    if (pa == NULL)
    {
        fprintf(stderr, "ERROR: Failed to connect pulseaudio server: %s\n", pa_strerror(pa_errno));
        return 1;
    }

    char data[DATA_SIZE];
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
                pa_result = pa_simple_write(pa, pydata->data, sizeof(pydata->data), &pa_errno);
                if (pa_result < 0)
                {
                    fprintf(stderr, "ERROR: Failed to write data to pulseaudio: %s\n", pa_strerror(pa_errno));
                    return 1;
                }
            }
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

    pa_simple_free(pa);
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
    PYMUSIC->release();
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

    // play music
    std::cerr << "open play music ... ";
    if (!PYMUSIC->open(SSM_READ))
    {
        throw std::runtime_error("[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open play music on ssm.\n");
    }
    else
    {
        std::cerr << "OK.\n";
    }
}