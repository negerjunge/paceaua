#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <chrono>
#include <SDL2/SDL_ttf.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
}
#include <cstdio>
#include <cstdint>


#define BUFFER_SIZE 4096



bool apasareDeTaste ;


int bani()
{
    int chips = 50000;
    
    
    
    
    
    
    
    
    int multiplier[9] = {5, 10, 25, 50, 100, 500, 1000, 5000, 10000};
    int bet[5] = {250, 500, 1000, 2500, };
    return 0;
}














int main()
{
    
    AVCodec * codec = NULL;
    AVCodecContext * codecContext = NULL;
    FILE * porno;
    AVFrame * pornoFrame;
    AVPacket packet;
    av_init_packet(&packet);
    uint8_t inputBuffer[BUFFER_SIZE +  AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t buff[1024];
    memset(inputBuffer + BUFFER_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG4);
    if(codec == NULL)
    {
        std::cout << "PULA" << std::endl;
    }
    codecContext = avcodec_alloc_context3(codec);
    if(codecContext == NULL)
    {
        std::cout << "pula" << std::endl;
    }
    pornoFrame == av_frame_alloc();
    if(codec -> capabilities &AV_CODEC_CAP_TRUNCATED)
    {
        codecContext -> flags |= AV_CODEC_FLAG_TRUNCATED;
    }
    if(avcodec_open2(codecContext, codec, NULL) < 0)
    {
            std::cout << "fa urat" << std::endl;
    }
    




    SDL_Texture *pizdeBune[9] ={};
    SDL_Window *pizdePng;
    SDL_Init(SDL_INIT_VIDEO);
    pizdePng = SDL_CreateWindow("Pacanea Cu Desfranate", 0, 0, 1920, 1080, SDL_WINDOW_MAXIMIZED);
    SDL_Renderer *randat = NULL;
    randat = SDL_CreateRenderer(pizdePng, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG);
    for(int i = 1; i <= 9 ; i++)
    {
      std::string folderPoze = "/home/theasspounder/vsc/pacaneav2/paceaua/pozePacaneaDesfranata/";
      folderPoze = folderPoze + std::to_string(i) + ".png"; 
      SDL_Surface*lemonParty = IMG_Load(folderPoze.c_str());
      if(!lemonParty) std::cout << "pulaMea" << std::endl;
       pizdeBune[i - 1] = SDL_CreateTextureFromSurface(randat, lemonParty);
       SDL_FreeSurface(lemonParty);
    }
    SDL_Rect whores;
     TTF_Init();
     TTF_Font *fontChips;
     fontChips = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 15);
     SDL_Surface *textChips;
     textChips =  TTF_RenderText_Solid(fontChips, "Ai Atatea Belite: ", {223, 194, 123});
     SDL_Texture *texChips;
     texChips = SDL_CreateTextureFromSurface(randat, textChips);
     

while(1 == 1)
{
    SDL_Event pimp;
    SDL_PollEvent(& pimp);
    if(pimp.type == SDL_QUIT)    
{
    break;   
}
    if(pimp.type == SDL_KEYDOWN)
    {
    apasareDeTaste = false;
                if(pimp.type == SDL_KEYDOWN)
                 {
                     apasareDeTaste = true;
                 }
    }
    
    
    if(pimp.type == SDL_KEYUP)
    {
        (char)pimp.key.keysym.sym;
            if(apasareDeTaste == true)
            {
                 std::cout << (char)pimp.key.keysym.sym << std::endl;
                apasareDeTaste = false;
                 apasareDeTaste != true;
            }
    }

SDL_RenderClear(randat);
SDL_Rect whores;
whores.h = 200;
whores.w = 200;
whores.x = 760;
whores.y = 440;
SDL_Rect whoresText;
whoresText.h = 75;
whoresText.w = 500;
whoresText.x = 560;
whoresText.y = 240;
SDL_RenderCopy(randat, pizdeBune[2], NULL, &whores);
SDL_RenderCopy(randat, texChips, NULL, &whoresText);
SDL_RenderPresent(randat);

}
    for(int i = 0; i <= 8; i++)
    {
    SDL_DestroyTexture(pizdeBune[i]);
    }

    SDL_DestroyRenderer(randat);
    SDL_DestroyWindow(pizdePng);   
    SDL_Quit(); 
    return 0;
}