#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ssm.hpp>
#include <pulse/error.h>  /* pulseaudio */
#include <pulse/simple.h> /* pulseaudio */
#include "recordmusic.hpp"

#define APP_NAME "pulseaudio_sample"
#define STREAM_NAME "rec"
#define DATA_SIZE 1024

static void ctrlC(int aStatus);
static void setSigInt();
static void Terminate(void);
static void setupSSM(void);

static int gShutOff = 0;
static unsigned int dT = 10; // 10ms

static SSMApi<rec_music, rec_music_property> *REMUSIC;

int main(int aArgc, char *aArgv[])
{
    SSMApi<rec_music, rec_music_property> remusic(RECORDMUSIC_SNAME, 0);
    REMUSIC = &remusic;
    rec_music *redata = &remusic.data;
    int pa_errno, pa_result, written_bytes;

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = 48000;
    ss.channels = 1;
    pa_simple *pa = pa_simple_new(NULL, APP_NAME, PA_STREAM_RECORD, NULL, STREAM_NAME, &ss, NULL, NULL, &pa_errno);
    if (pa == NULL)
    {
        fprintf(stderr, "ERROR: Failed to connect pulseaudio server: %s\n", pa_strerror(pa_errno));
        return 1;
    }
    try
    {
        setupSSM();
        setSigInt();

        char data[DATA_SIZE];
        while (!gShutOff)
        {
            pa_result = pa_simple_read(pa, data, DATA_SIZE, &pa_errno);
            if (pa_result < 0)
            {
                fprintf(stderr, "ERROR: Failed to read data from pulseaudio: %s\n", pa_strerror(pa_errno));
                return EXIT_FAILURE;
            }

            memcpy(data,redata->data,strlen(redata->data)+1);
            /*written_bytes = write(STDOUT_FILENO, data, DATA_SIZE);
            if (written_bytes < DATA_SIZE)
            {
                fprintf(stderr, "ERROR: Failed to write data to stdout: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }*/
            remusic.write();
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
    REMUSIC->release();
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

    // Localizer1作成
    std::cerr << "create remusic ... ";
    if (!REMUSIC->create(1, (double)dT / 1000.0))
    {
        throw std::runtime_error("[\033[1m\033[31mERROR\033[30m\033[0m]:fail to create remusic on ssm.\n");
    }
    else
    {
        std::cerr << "OK.\n";
    }
}