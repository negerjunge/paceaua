#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <cstdio>
#include <error.h>
#include <cstdlib>
#include <filesystem>
#include <chrono>

bool runMode = false;
bool ifBet = false;
bool apasareDeTaste;
int belite;
int fps = 350;
uint32_t startTimeF = 0;
uint32_t endTimeF = 0;
uint32_t deltaF = 0;
uint32_t durataF = 1000 / 350;
uint8_t clipRawOut[4 * 640 * 360];
uint8_t clipPerf[4 * 640 * 360];
Uint32 waitTime = 50;
uint8_t clipPacRawOut1[4 * 374 * 112];
uint8_t clipPacRawOut2[4 * 374 * 112];
uint8_t clipPacRawOut3[4 * 374 * 112];
uint8_t framesPerformance[4 * 640 * 360];
std::chrono::system_clock::time_point timepointEnter1;
std::chrono::system_clock::time_point timepointEnter2;
std::chrono::system_clock::time_point timepointEnter3;
std::chrono::system_clock::time_point timepointEnter12;
std::chrono::system_clock::time_point timepointEnter23;
std::chrono::system_clock::time_point timepointEnter34;


static AVCodecContext *video_dec_ctx = NULL;
static uint8_t *video_dst_data[4] = {NULL};
static int video_dst_linesize[4];
static int video_dst_bufsize;
static int video_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;
static FILE *video_dst_file;

static int decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;
    *got_frame = 0;
    if (pkt.stream_index == video_stream_idx)
    {
        /* decode video frame */
        ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0)
        {
            fprintf(stderr, "Error decoding video frame \n");
            return ret;
        }
        if (*got_frame)
        {
            size_t bitmap_size = 4 * video_dec_ctx->width * video_dec_ctx->height;
            uint8_t *bitmap = (uint8_t *)malloc(bitmap_size);
            int rgb_stride[4] = {-(video_dec_ctx->width * 4), 0, 0, 0};
            uint8_t *rgb_dest[4] = {bitmap + (video_dec_ctx->height - 1) * video_dec_ctx->width * 4, 0, 0, 0};

            SwsContext *sws_ctx = sws_getContext(
                video_dec_ctx->width,
                video_dec_ctx->height,
                video_dec_ctx->pix_fmt,
                video_dec_ctx->width,
                video_dec_ctx->height,
                AV_PIX_FMT_RGBA,
                SWS_AREA,
                0,
                0,
                0);
            sws_scale(
                sws_ctx,
                frame->data,
                frame->linesize,
                0,
                video_dec_ctx->height,
                rgb_dest,
                rgb_stride);

            uint8_t *rev = (uint8_t *)malloc(bitmap_size);
            for (size_t i = 0; i < video_dec_ctx->height; ++i)
            {
                uint8_t *line = (uint8_t *)malloc(4 * video_dec_ctx->width);
                memcpy(line, (bitmap + i * 4 * video_dec_ctx->width), 4 * video_dec_ctx->width);
                memcpy((rev + bitmap_size - 4 * video_dec_ctx->width) - i * 4 * video_dec_ctx->width, line, 4 * video_dec_ctx->width);
                free(line);
            }

            sws_freeContext(sws_ctx);
            memcpy(framesPerformance, rev, bitmap_size);
            free(rev);
            free(bitmap);
        }
    }

    if (*got_frame)
        av_frame_unref(frame);
    return decoded;
}
static int open_codec_context(int *stream_idx,
                              AVFormatContext *fmt_ctx, enum AVMediaType type, const char *src_filename)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
        return ret;
    }
    else
    {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];
        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec)
        {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }
        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", "1", 0);
        if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0)
        {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
    }
    return 0;
}

static int get_format_from_sample_fmt(const char **fmt,
                                      enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry
    {
        enum AVSampleFormat sample_fmt;
        const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        {AV_SAMPLE_FMT_U8, "u8", "u8"},
        {AV_SAMPLE_FMT_S16, "s16be", "s16le"},
        {AV_SAMPLE_FMT_S32, "s32be", "s32le"},
        {AV_SAMPLE_FMT_FLT, "f32be", "f32le"},
        {AV_SAMPLE_FMT_DBL, "f64be", "f64le"},
    };
    *fmt = NULL;
    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++)
    {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt)
        {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }
    fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}

void set_pixel(SDL_Renderer *rend, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawColor(rend, r, g, b, a);
    SDL_RenderDrawPoint(rend, x, y);
}

void draw_circle(SDL_Renderer *surface, int n_cx, int n_cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    // if the first pixel in the screen is represented by (0,0) (which is in sdl)
    // remember that the beginning of the circle is not in the middle of the pixel
    // but to the left-top from it:

    double error = (double)-radius;
    double x = (double)radius - 0.5;
    double y = (double)0.5;
    double cx = n_cx - 0.5;
    double cy = n_cy - 0.5;

    while (x >= y)
    {
        set_pixel(surface, (int)(cx + x), (int)(cy + y), r, g, b, a);
        set_pixel(surface, (int)(cx + y), (int)(cy + x), r, g, b, a);

        if (x != 0)
        {
            set_pixel(surface, (int)(cx - x), (int)(cy + y), r, g, b, a);
            set_pixel(surface, (int)(cx + y), (int)(cy - x), r, g, b, a);
        }

        if (y != 0)
        {
            set_pixel(surface, (int)(cx + x), (int)(cy - y), r, g, b, a);
            set_pixel(surface, (int)(cx - y), (int)(cy + x), r, g, b, a);
        }

        if (x != 0 && y != 0)
        {
            set_pixel(surface, (int)(cx - x), (int)(cy - y), r, g, b, a);
            set_pixel(surface, (int)(cx - y), (int)(cy - x), r, g, b, a);
        }

        error += y;
        ++y;
        error += y;

        if (error >= 0)
        {
            --x;
            error -= x;
            error -= x;
        }
        /*
		// sleep for debug
		SDL_RenderPresent(gRenderer);
		std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
		*/
    }
}

void fill_circle(SDL_Renderer *renderer, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    // Note that there is more to altering the bitrate of this
    // method than just changing this value.  See how pixels are
    // altered at the following web page for tips:
    //   http://www.libsdl.org/intro.en/usingvideo.html
    static const int BPP = 4;

    //double ra = (double)radius;

    for (double dy = 1; dy <= radius; dy += 1.0)
    {
        // This loop is unrolled a bit, only iterating through half of the
        // height of the circle.  The result is used to draw a scan line and
        // its mirror image below it.

        // The following formula has been simplified from our original.  We
        // are using half of the width of the circle because we are provided
        // with a center and we need left/right coordinates.

        double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
        int x = cx - dx;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawLine(renderer, cx - dx, cy + dy - radius, cx + dx, cy + dy - radius);
        SDL_RenderDrawLine(renderer, cx - dx, cy - dy + radius, cx + dx, cy - dy + radius);
    }
}

static AVFormatContext *fmt_ctx = NULL;
static AVStream *video_stream = NULL;
static const char *src_filename = NULL;
static const char *video_dst_filename = NULL;
int ret = 0, got_frame;

void ffmpeg_init()
{
    /* open input file, and allocate format context */
    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0)
    {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        exit(1);
    }
    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0)
    {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
    if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO, src_filename) >= 0)
    {
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = video_stream->codec;
        /* allocate image where the decoded image will be put */
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             video_dec_ctx->width, video_dec_ctx->height,
                             video_dec_ctx->pix_fmt, 1);
        if (ret < 0)
        {
            fprintf(stderr, "Could not allocate raw video buffer\n");
            return;
        }
        video_dst_bufsize = ret;
    }
    av_dump_format(fmt_ctx, 0, src_filename, 0);
    frame = av_frame_alloc();
    if (!frame)
    {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        return;
    }
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
}

void ffmpeg_free()
{
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
}

int main()
{
    using namespace std::filesystem;
    int belite = 50000;
    int bet[4] = {250, 500, 1000, 2500};
    int betAr = 0;
    int pozeAr0 = rand() % 9;
    int pozeAr1 = rand() % 9;
    int pozeAr2 = rand() % 9;
    int multi[9] = {5, 10, 25, 50, 100, 500, 1000, 5000, 10000};
    uint8_t decoderClip[4 * 640 * 360];

    srand(time(NULL));

    src_filename = "clipDesfranat/porn.mp4";
    ffmpeg_init();

//baga sample audio ca te omor    
    Mix_Init(MIX_INIT_MP3);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Renderer *randat = NULL;
    SDL_Window *pizdePng;
    pizdePng = SDL_CreateWindow("Pacanea Cu Desfranate", 0, 0, 1920, 1080, SDL_WINDOW_SHOWN);
    IMG_Init(IMG_INIT_PNG);
    randat = SDL_CreateRenderer(pizdePng, -1, SDL_RENDERER_ACCELERATED);
    SDL_Rect whores;
    TTF_Init();
    TTF_Font *fontChips;
    fontChips = TTF_OpenFont((path("fonts") / ("Hack-Bold.ttf")).c_str(), 15);
    SDL_Surface *textChips;
    std::string cateBeliteAm;
    cateBeliteAm = "Ai Atatea Belite: ";
    cateBeliteAm += std::to_string(belite);
    textChips = TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
    SDL_Texture *texChips;
    texChips = SDL_CreateTextureFromSurface(randat, textChips);

    SDL_Rect textPerformance;
    SDL_Surface *surfTextPerf;
    std::string stringTextPerf = "CHOOSE EXECUTION MODE: ";
    surfTextPerf = TTF_RenderText_Solid(fontChips, stringTextPerf.c_str(), {223, 194, 123});
    SDL_Texture *texTextPerf;
    texTextPerf = SDL_CreateTextureFromSurface(randat, surfTextPerf);

    SDL_Rect textPHP;
    SDL_Surface *surfTextPHP;
    std::string stringTextPHP = "High Performance Mode: 'On The Fly Decoding' ";
    surfTextPHP = TTF_RenderText_Solid(fontChips, stringTextPHP.c_str(), {223, 194, 123});
    SDL_Texture *texTextPHP;
    texTextPHP = SDL_CreateTextureFromSurface(randat, surfTextPHP);

    SDL_Rect textPLP;
    SDL_Surface *surfTextPLP;
    std::string stringTextPLP = "Low Performance Mode: 'Normal Decoding(needs loading)' ";
    surfTextPLP = TTF_RenderText_Solid(fontChips, stringTextPLP.c_str(), {223, 194, 123});
    SDL_Texture *texTextPLP;
    texTextPLP = SDL_CreateTextureFromSurface(randat, surfTextPLP);

    while (1 == 1)
    {
        SDL_Event pimp1;
        SDL_PollEvent(&pimp1);
        if (pimp1.type == SDL_QUIT)
        {
            break;
        }

        if (pimp1.type == SDL_KEYDOWN)
        {
            apasareDeTaste = false;
            if (pimp1.type == SDL_KEYDOWN)
            {
                apasareDeTaste = true;
            }
        }

        if (pimp1.type == SDL_KEYUP)
        {
            (char)pimp1.key.keysym.sym;
            if (apasareDeTaste == true)
            {
                apasareDeTaste = false;
                apasareDeTaste != true;
                if (pimp1.key.keysym.sym == SDL_KeyCode::SDLK_UP)
                {
                    runMode = !runMode;
                    std::cout << "runmode = true" << std::endl;
                }
                if (pimp1.key.keysym.sym == SDL_KeyCode::SDLK_DOWN)
                {
                    runMode = !runMode;
                    std::cout << "runmode = false" << std::endl;
                }
                if (pimp1.key.keysym.sym == SDL_KeyCode::SDLK_RETURN)
                {
                    std::cout << "break" << std::endl;
                    break;
                }
            }
        }
        

        SDL_SetRenderDrawColor(randat, 0, 0, 0, 255);
        SDL_RenderClear(randat);
        SDL_Rect textPerformance;
        textPerformance.h = 75;
        textPerformance.w = 500;
        textPerformance.x = 260;
        textPerformance.y = 240;

        SDL_Rect textPHP;
        textPHP.h = 100;
        textPHP.w = 600;
        textPHP.x = 400;
        textPHP.y = 500;

        SDL_Rect textPLP;
        textPLP.h = 100;
        textPLP.w = 600;
        textPLP.x = 400;
        textPLP.y = 700;

        SDL_RenderCopy(randat, texTextPLP, NULL, &textPLP);
        SDL_RenderCopy(randat, texTextPerf, NULL, &textPerformance);
        SDL_RenderCopy(randat, texTextPHP, NULL, &textPHP);
        if (runMode == false)
        {
            SDL_SetRenderDrawColor(randat, 255, 255, 255, 255);
            draw_circle(randat, 300, 550, 30, 0xFF, 0xFF, 0xFF, 0xFF);
            fill_circle(randat, 300, 550, 30, 0xFF, 0xFF, 0xFF, 0xFF);
        }
        if (runMode == true)
        {
            SDL_SetRenderDrawColor(randat, 255, 255, 255, 255);
            draw_circle(randat, 300, 750, 30, 0xFF, 0xFF, 0xFF, 0xFF);
            fill_circle(randat, 300, 750, 30, 0xFF, 0xFF, 0xFF, 0xFF);
        }
        SDL_RenderPresent(randat);
    }

    if (runMode == true)
    {
        video_dst_filename = "clip.out";
        /* register all formats and codecs */
        video_dst_file = fopen(video_dst_filename, "wb");
        while (av_read_frame(fmt_ctx, &pkt) >= 0)
        {
            AVPacket orig_pkt = pkt;
            do
            {
                ret = decode_packet(&got_frame, 0);
                if (ret < 0)
                    break;
                pkt.data += ret;
                pkt.size -= ret;
                if (got_frame)
                {
                    fwrite(framesPerformance, 1, 4 * 640 * 360, video_dst_file);
                }
            } while (pkt.size > 0);

            av_free_packet(&orig_pkt);
        }

        pkt.data = NULL;
        pkt.size = 0;
        do
        {
            decode_packet(&got_frame, 1);
            if (got_frame)
            {
                fwrite(framesPerformance, 1, 4 * 640 * 360, video_dst_file);
            }
        } while (got_frame);
        fclose(video_dst_file);
    }

    FILE *clipOutput;
    clipOutput = fopen("clip.out", "rb");
    if (!clipOutput)
        std::cout << "pulapizda" << std::endl;
    else
    {
        fread(clipRawOut, 1, 4 * 640 * 360, clipOutput);
    fseek(clipOutput, 921600 * 5, SEEK_CUR);
    fread(clipRawOut, 1, 4 * 640 * 360, clipOutput);
    }

    FILE *clipOutPac1;
    clipOutPac1 = fopen((path("clipDesfranat") / ("pacVid") / "outPac1.bin").c_str(), "rb");
    fread(clipPacRawOut1, 1, 4 * 374 * 112, clipOutPac1);
    if (!clipOutPac1)
        std::cout << "mori1" << std::endl;

    FILE *clipOutPac2;
    clipOutPac2 = fopen((path("clipDesfranat") / ("pacVid") / "outPac2.bin").c_str(), "rb");
    fread(clipPacRawOut2, 1, 4 * 374 * 112, clipOutPac2);
    if (!clipOutPac2)
        std::cout << "mori2" << std::endl;

    FILE *clipOutPac3;
    clipOutPac3 = fopen((path("clipDesfranat") / ("pacVid") / "outPac3.bin").c_str(), "rb");
    fread(clipPacRawOut3, 1, 4 * 374 * 112, clipOutPac3);
    if (!clipOutPac3)
        std::cout << "mori3" << std::endl;

    SDL_Texture *pizdeBune[9] = {};
    for (int i = 1; i <= 9; i++)
    {
        std::string folderPoze = path("pozePacaneaDesfranata") / (std::to_string(i) + ".png");
        //folderPoze = folderPoze + std::to_string(i) + ".png";
        SDL_Surface *lemonParty = IMG_Load(folderPoze.c_str());
        if (!lemonParty)
            std::cout << "pulaMea" << std::endl;
        pizdeBune[i - 1] = SDL_CreateTextureFromSurface(randat, lemonParty);
        SDL_FreeSurface(lemonParty);
    }
    SDL_Surface *framesClipInfo;
    framesClipInfo = SDL_CreateRGBSurfaceFrom(clipRawOut, 640, 360, 32, 2560, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *textureClip;
    textureClip = SDL_CreateTextureFromSurface(randat, framesClipInfo);

    SDL_Rect pac1;
    SDL_Surface *clipPac1;
    clipPac1 = SDL_CreateRGBSurfaceFrom(clipPacRawOut1, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *texClipPac1;
    texClipPac1 = SDL_CreateTextureFromSurface(randat, clipPac1);

    SDL_Rect pac2;
    SDL_Surface *clipPac2;
    clipPac2 = SDL_CreateRGBSurfaceFrom(clipPacRawOut2, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *texClipPac2;
    texClipPac2 = SDL_CreateTextureFromSurface(randat, clipPac2);

    SDL_Rect pac3;
    SDL_Surface *clipPac3;
    clipPac3 = SDL_CreateRGBSurfaceFrom(clipPacRawOut3, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture *texClipPac3;
    texClipPac3 = SDL_CreateTextureFromSurface(randat, clipPac3);

    SDL_Rect whoresBet;
    SDL_Surface *textelBet;
    std::string textel1Bet = "Bet Nenorocit: ";
    textel1Bet += std::to_string(bet[betAr]);
    textelBet = TTF_RenderText_Solid(fontChips, textel1Bet.c_str(), {223, 194, 123});
    SDL_Texture *texBet;
    texBet = SDL_CreateTextureFromSurface(randat, textelBet);

    SDL_Rect textWin;
    SDL_Surface *surfaceWin;
    std::string stringWin = " ";

    surfaceWin = TTF_RenderText_Solid(fontChips, stringWin.c_str(), {223, 194, 123});
    SDL_Texture *textureWin;
    textureWin = SDL_CreateTextureFromSurface(randat, surfaceWin);
    SDL_SetRenderDrawColor(randat, 255, 255, 255, 255);

    while (1 == 1)
    {

        if (startTimeF == 0)
        {
            startTimeF = SDL_GetTicks();
        }
        else
        {
            deltaF = endTimeF - startTimeF;
        }
        if (deltaF < durataF)
        {
            SDL_Delay(durataF - deltaF);
        }

        SDL_Event pimp;
        SDL_PollEvent(&pimp);
        if (pimp.type == SDL_QUIT)
        {
            break;
        }

        if (pimp.type == SDL_KEYDOWN)
        {
            apasareDeTaste = false;
            if (pimp.type == SDL_KEYDOWN)
            {
                apasareDeTaste = true;
            }
        }

        if (pimp.type == SDL_KEYUP)
        {
            (char)pimp.key.keysym.sym;
            if (apasareDeTaste == true)
            {
                apasareDeTaste = false;
                apasareDeTaste != true;
                if (pimp.key.keysym.sym == SDL_KeyCode::SDLK_RETURN)
                {
                    if (belite <= 0)
                        return 0;
                    std::cout << "penis" << std::endl;
                    SDL_DestroyTexture(texChips);
                    SDL_FreeSurface(textChips);
                    belite -= bet[betAr];
                    cateBeliteAm = "Ai Atatea Belite: ";
                    cateBeliteAm += std::to_string(belite);
                    textChips = TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
                    texChips = SDL_CreateTextureFromSurface(randat, textChips);
                    pozeAr0 = rand() % 9;
                    pozeAr1 = rand() % 9;
                    pozeAr2 = rand() % 9;
                    SDL_DestroyTexture(textureWin);
                    stringWin = " ";
                    SDL_FreeSurface(surfaceWin);
                    surfaceWin = TTF_RenderText_Solid(fontChips, stringWin.c_str(), {223, 194, 123});
                    textureWin = SDL_CreateTextureFromSurface(randat, surfaceWin);
                    ifBet = false;
                    timepointEnter1 = std::chrono::system_clock::now();
                    timepointEnter2 = std::chrono::system_clock::now();
                    timepointEnter3 = std::chrono::system_clock::now();
                    std::cout << "timepoint" << std::endl;
                }
                else if (pimp.key.keysym.sym == SDL_KeyCode::SDLK_LEFT)
                {
                    betAr--;
                    if (betAr < 0)
                    {
                        betAr = 0;
                    }
                    std::cout << "merge betu coaie" << std::endl;
                    SDL_DestroyTexture(texBet);
                    SDL_FreeSurface(textelBet);
                    textel1Bet = "Bet Nenorocit: ";
                    textel1Bet += std::to_string(bet[betAr]);
                    textelBet = TTF_RenderText_Solid(fontChips, textel1Bet.c_str(), {223, 192, 123});
                    texBet = SDL_CreateTextureFromSurface(randat, textelBet);
                }
                else if (pimp.key.keysym.sym == SDL_KeyCode::SDLK_RIGHT)
                {
                    betAr++;
                    if (betAr == 4)
                    {
                        betAr = 0;
                    }
                    std::cout << "merge betu coaie" << std::endl;
                    SDL_DestroyTexture(texBet);
                    SDL_FreeSurface(textelBet);
                    textel1Bet = "Bet Nenorocit: ";
                    textel1Bet += std::to_string(bet[betAr]);

                    textelBet = TTF_RenderText_Solid(fontChips, textel1Bet.c_str(), {223, 192, 123});
                    texBet = SDL_CreateTextureFromSurface(randat, textelBet);
                }
                if (pimp.key.keysym.sym == SDL_KeyCode::SDLK_f)
                {
                    if (fps == 15)
                    {
                        fps = 350;
                        durataF = 1000 / 350;
                    }
                    else if (fps == 350)
                    {
                        fps = 15;
                        durataF = 1000 / 15;
                    }
                }
            }
        }

        if (pozeAr0 == pozeAr1 && pozeAr1 == pozeAr2)
        {

            stringWin = "Dai La Muie";

            SDL_DestroyTexture(textureWin);
            SDL_FreeSurface(surfaceWin);

            surfaceWin = TTF_RenderText_Solid(fontChips, stringWin.c_str(), {223, 194, 123});
            textureWin = SDL_CreateTextureFromSurface(randat, surfaceWin);
        }

        if (pozeAr0 == pozeAr1 && pozeAr1 == pozeAr2 && ifBet == false)
        {
            ifBet = true;
            std::cout << "dai la muie responsabil" << std::endl;
            belite += bet[betAr] * multi[pozeAr0];
            cateBeliteAm;
            cateBeliteAm = "Ai Atatea Belite: ";
            cateBeliteAm += std::to_string(belite);
            textChips = TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
            texChips = SDL_CreateTextureFromSurface(randat, textChips);
        }

        SDL_RenderClear(randat);

        SDL_Rect whores0;
        whores0.h = 200;
        whores0.w = 200;
        whores0.x = 420;
        whores0.y = 550;
        SDL_Rect whores1;
        whores1.h = 200;
        whores1.w = 200;
        whores1.x = 940;
        whores1.y = 550;
        SDL_Rect whores2;
        whores2.h = 200;
        whores2.w = 200;
        whores2.x = 1380;
        whores2.y = 550;
        SDL_Rect whoresText;
        whoresText.h = 75;
        whoresText.w = 500;
        whoresText.x = 260;
        whoresText.y = 240;
        SDL_Rect randClip;
        randClip.h = 1080;
        randClip.w = 1920;
        randClip.x = 0;
        randClip.y = 0;

        SDL_Rect whoresBet;
        whoresBet.h = 200;
        whoresBet.w = 400;
        whoresBet.x = 20;
        whoresBet.y = 20;

        SDL_Rect textWin;
        textWin.h = 200;
        textWin.w = 200;
        textWin.x = 700;
        textWin.y = 550;

        SDL_Rect pac1;
        pac1.h = 200;
        pac1.w = 200;
        pac1.x = 420;
        pac1.y = 550;

        SDL_Rect pac2;
        pac2.h = 200;
        pac2.w = 200;
        pac2.x = 940;
        pac2.y = 550;

        SDL_Rect pac3;
        pac3.h = 200;
        pac3.w = 200;
        pac3.x = 1380;
        pac3.y = 550;

        if (runMode == false)
        {
            if(av_read_frame(fmt_ctx, &pkt) >= 0)
            {
                AVPacket orig_pkt = pkt;
                do
                {
                    ret = decode_packet(&got_frame, 0);
                    if (ret < 0)
                        break;
                    pkt.data += ret;
                    pkt.size -= ret;
                    if (got_frame)
                    {
                        SDL_DestroyTexture(textureClip);
                        SDL_FreeSurface(framesClipInfo);
                        framesClipInfo = SDL_CreateRGBSurfaceFrom(framesPerformance, 640, 360, 32, 2560, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
                        textureClip = SDL_CreateTextureFromSurface(randat, framesClipInfo);
                    }
                } while (pkt.size > 0);
                
                av_free_packet(&orig_pkt);
            }
        }
        else
        {
            if (fread(clipRawOut, 1, 4 * 640 * 360, clipOutput) == 0)
            {
                fseek(clipOutput, 921600 * 45, SEEK_SET);
            }
            SDL_DestroyTexture(textureClip);
            SDL_FreeSurface(framesClipInfo);
            framesClipInfo = SDL_CreateRGBSurfaceFrom(clipRawOut, 640, 360, 32, 2560, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
            textureClip = SDL_CreateTextureFromSurface(randat, framesClipInfo);
        }

        if (fread(clipPacRawOut1, 1, 4 * 374 * 112, clipOutPac1) == 0)
        {
            fseek(clipOutPac1, 167552 * 0, SEEK_SET);
        }
        SDL_DestroyTexture(texClipPac1);
        SDL_FreeSurface(clipPac1);
        clipPac1 = SDL_CreateRGBSurfaceFrom(clipPacRawOut1, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        texClipPac1 = SDL_CreateTextureFromSurface(randat, clipPac1);

        if (fread(clipPacRawOut2, 1, 4 * 374 * 112, clipOutPac2) == 0)
        {
            fseek(clipOutPac2, 167552 * 0, SEEK_SET);
        }
        SDL_DestroyTexture(texClipPac2);
        SDL_FreeSurface(clipPac2);
        clipPac2 = SDL_CreateRGBSurfaceFrom(clipPacRawOut2, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        texClipPac2 = SDL_CreateTextureFromSurface(randat, clipPac2);

        if (fread(clipPacRawOut3, 1, 4 * 374 * 112, clipOutPac3) == 0)
        {
            fseek(clipOutPac3, 167552 * 0, SEEK_SET);
        }
        SDL_DestroyTexture(texClipPac3);
        SDL_FreeSurface(clipPac3);
        clipPac3 = SDL_CreateRGBSurfaceFrom(clipPacRawOut3, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        texClipPac3 = SDL_CreateTextureFromSurface(randat, clipPac3);

        timepointEnter12 = std::chrono::system_clock::now();
        timepointEnter23 = std::chrono::system_clock::now();
        timepointEnter34 = std::chrono::system_clock::now();
        
        SDL_RenderCopy(randat, textureClip, NULL, &randClip);
        SDL_RenderCopy(randat, pizdeBune[pozeAr0], NULL, &whores0);
        SDL_RenderCopy(randat, pizdeBune[pozeAr1], NULL, &whores1);
        SDL_RenderCopy(randat, pizdeBune[pozeAr2], NULL, &whores2);
        SDL_RenderCopy(randat, texChips, NULL, &whoresText);
        SDL_RenderCopy(randat, texBet, NULL, &whoresBet);
        SDL_RenderCopy(randat, textureWin, NULL, &textWin);
        if (std::chrono::duration_cast<std::chrono::milliseconds>(timepointEnter12 - timepointEnter1).count() < 1000)
        {
            SDL_RenderCopy(randat, texClipPac1, NULL, &pac1);
        }
        if (std::chrono::duration_cast<std::chrono::seconds>(timepointEnter23 - timepointEnter2).count() < 2)
        {
            SDL_RenderCopy(randat, texClipPac2, NULL, &pac2);
        }
        if (std::chrono::duration_cast<std::chrono::milliseconds>(timepointEnter34 - timepointEnter3).count() < 3000)
        {
            SDL_RenderCopy(randat, texClipPac3, NULL, &pac3);
        }
        SDL_RenderPresent(randat);
        startTimeF = endTimeF;
        endTimeF = SDL_GetTicks();
    }
    for (int i = 0; i <= 8; i++)
    {
        SDL_DestroyTexture(pizdeBune[i]);
    }

    ffmpeg_free();
    SDL_DestroyRenderer(randat);
    SDL_DestroyWindow(pizdePng);
    SDL_Quit();
    Mix_Quit();
    return 0;
}