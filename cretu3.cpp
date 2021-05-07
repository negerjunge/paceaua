#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <error.h>
#include <filesystem>
#include <chrono>

bool ifBet = false;
bool apasareDeTaste; 
int belite;
int fps = 350;
uint32_t startTimeF = 0;
uint32_t endTimeF = 0;
uint32_t deltaF = 0;
uint32_t durataF = 1000/350;
uint8_t clipRawOut[4 * 640 * 360];
Uint32 waitTime = 50;
uint8_t clipPacRawOut1[4 * 374 * 112];
uint8_t clipPacRawOut2[4 * 374 * 112];
uint8_t clipPacRawOut3[4 * 374 * 112];
std::chrono::system_clock::time_point timepointEnter1;
std::chrono::system_clock::time_point timepointEnter2;
std::chrono::system_clock::time_point timepointEnter3;
std::chrono::system_clock::time_point timepointEnter12;
std::chrono::system_clock::time_point timepointEnter23;
std::chrono::system_clock::time_point timepointEnter34;


int main()
{ 
    std::filesystem::path("windows") /"runWindows";
    int belite = 50000;
    int bet[4] = {250, 500, 1000, 2500};
    int betAr = 0;
    int pozeAr0 = rand() % 9;
    int pozeAr1 = rand() % 9;
    int pozeAr2 = rand() % 9;
    int multi[9] = {5, 10, 25, 50, 100, 500, 1000, 5000, 10000};
    
    srand(time(NULL));

    FILE * clipOutput;
    clipOutput = fopen("clipDesfranat/out.bin", "rb"); 
    if(!clipOutput) std::cout << "pulapizda" << std::endl;
    fread(clipRawOut, 1, 4 * 640 * 360, clipOutput);
    fseek(clipOutput, 921600 * 5, SEEK_CUR);
    fread(clipRawOut, 1, 4 * 640 * 360, clipOutput);
    
    FILE * clipOutPac1;
    clipOutPac1 = fopen("clipDesfranat/pacVid/outPac1.bin", "rb");
    fread(clipPacRawOut1, 1, 4 * 374 * 112, clipOutPac1);
    if(!clipOutPac1) std::cout << "mori1" << std::endl;

    FILE * clipOutPac2;
    clipOutPac2 = fopen("clipDesfranat/pacVid/outPac2.bin", "rb");
    fread(clipPacRawOut2, 1, 4 * 374 * 112, clipOutPac2);
    if(!clipOutPac2) std::cout << "mori2" << std::endl;

    FILE * clipOutPac3;
    clipOutPac3 = fopen("clipDesfranat/pacVid/outPac3.bin", "rb");
    fread(clipPacRawOut3, 1, 4 * 374 * 112, clipOutPac3);
    if(!clipOutPac3) std::cout << "mori3" << std::endl;

    SDL_Texture *pizdeBune[9] ={};
    SDL_Window *pizdePng;
    SDL_Init(SDL_INIT_VIDEO);
    pizdePng = SDL_CreateWindow("Pacanea Cu Desfranate", 0, 0, 1920, 1080, SDL_WINDOW_SHOWN);
    SDL_Renderer *randat = NULL;
    randat = SDL_CreateRenderer(pizdePng, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG);
    for(int i = 1; i <= 9 ; i++)
    {
      std::string folderPoze = "pozePacaneaDesfranata/";
      folderPoze = folderPoze + std::to_string(i) + ".png"; 
      SDL_Surface *lemonParty = IMG_Load(folderPoze.c_str());
      if(!lemonParty) std::cout << "pulaMea" << std::endl;
       pizdeBune[i - 1] = SDL_CreateTextureFromSurface(randat, lemonParty);
       SDL_FreeSurface(lemonParty);
    }
    SDL_Rect whores;
     TTF_Init();
     TTF_Font *fontChips;
     fontChips = TTF_OpenFont("/usr/share/fonts/hack/Hack-Bold.ttf", 15);
     SDL_Surface *textChips;
     std::string cateBeliteAm;
     cateBeliteAm = "Ai Atatea Belite: ";
     cateBeliteAm += std::to_string(belite);
     textChips =  TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
     SDL_Texture *texChips;
     texChips = SDL_CreateTextureFromSurface(randat, textChips);
    
    SDL_Surface * framesClipInfo;
    framesClipInfo = SDL_CreateRGBSurfaceFrom(clipRawOut, 640, 360, 32, 2560, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture * textureClip;
    textureClip = SDL_CreateTextureFromSurface(randat, framesClipInfo);

    SDL_Rect pac1;
    SDL_Surface * clipPac1;
    clipPac1 = SDL_CreateRGBSurfaceFrom(clipPacRawOut1, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture * texClipPac1;
    texClipPac1 = SDL_CreateTextureFromSurface(randat, clipPac1);

    SDL_Rect pac2;
    SDL_Surface * clipPac2;
    clipPac2 = SDL_CreateRGBSurfaceFrom(clipPacRawOut2, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture * texClipPac2;
    texClipPac2 = SDL_CreateTextureFromSurface(randat, clipPac2);

    SDL_Rect pac3;
    SDL_Surface * clipPac3;
    clipPac3 = SDL_CreateRGBSurfaceFrom(clipPacRawOut3, 374, 112, 32, 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    SDL_Texture * texClipPac3;
    texClipPac3 = SDL_CreateTextureFromSurface(randat, clipPac3);
 
    SDL_Rect whoresBet;
     SDL_Surface *textelBet;
     std::string textel1Bet = "Bet Nenorocit: ";
     textel1Bet += std::to_string(bet[betAr]);
     textelBet =  TTF_RenderText_Solid(fontChips, textel1Bet.c_str(), {223, 194, 123});
     SDL_Texture *texBet;
     texBet = SDL_CreateTextureFromSurface(randat, textelBet);

     SDL_Rect textWin;
     SDL_Surface *surfaceWin;
     std::string stringWin = " ";
     
     surfaceWin =  TTF_RenderText_Solid(fontChips, stringWin.c_str(), {223, 194, 123});
     SDL_Texture *textureWin;
     textureWin = SDL_CreateTextureFromSurface(randat, surfaceWin);

while(1 == 1)
{
    if(startTimeF == 0)
    {
        startTimeF = SDL_GetTicks();
    }
        else
            {
                deltaF = endTimeF - startTimeF;
            }
    if(deltaF < durataF)
    {
        SDL_Delay(durataF - deltaF);
    }

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
                 if(pimp.key.keysym.sym == SDL_KeyCode::SDLK_RETURN)
                 {
                            if(belite <= 0) return 0;
                     std::cout << "penis" << std::endl;
                    SDL_DestroyTexture(texChips);
                     SDL_FreeSurface(textChips);
                     belite -= bet[betAr];
                    cateBeliteAm = "Ai Atatea Belite: ";
                    cateBeliteAm  += std::to_string(belite);
                    textChips =  TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
                    texChips = SDL_CreateTextureFromSurface(randat, textChips);
                    pozeAr0 = rand() % 9;
                    pozeAr1 = rand() % 9;
                    pozeAr2 = rand() % 9;
                    SDL_DestroyTexture(textureWin);
                    stringWin = " ";
                    SDL_FreeSurface(surfaceWin);
                    surfaceWin =  TTF_RenderText_Solid(fontChips, stringWin.c_str(), {223, 194, 123});
                    textureWin = SDL_CreateTextureFromSurface(randat, surfaceWin);
                    ifBet = false;
                    timepointEnter1 = std::chrono::system_clock::now();
                    timepointEnter2 = std::chrono::system_clock::now();
                    timepointEnter3 = std::chrono::system_clock::now();
                    std::cout << "timepoint" << std::endl;
                 }
                else if(pimp.key.keysym.sym == SDL_KeyCode::SDLK_LEFT)
                 {
                    betAr --;    
                        if(betAr < 0)
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
                else if(pimp.key.keysym.sym == SDL_KeyCode::SDLK_RIGHT)
                 {
                     betAr ++;
                     if(betAr == 4)
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
                if(pimp.key.keysym.sym == SDL_KeyCode::SDLK_f)
                {
                          if(fps == 15)
                        {
                            fps = 350;
                            durataF = 1000/350;            
                        }
                        else if(fps == 350)
                        {
                            fps = 15;
                            durataF = 1000/15;
                        }
                                  
                } 
            }  
        }

    if(pozeAr0 == pozeAr1 && pozeAr1 == pozeAr2)
    {
        
        stringWin = "Dai La Muie";
     
        SDL_DestroyTexture(textureWin);
        SDL_FreeSurface(surfaceWin);

        surfaceWin =  TTF_RenderText_Solid(fontChips, stringWin.c_str(), {223, 194, 123});
        textureWin = SDL_CreateTextureFromSurface(randat, surfaceWin);
    }
    
        if(pozeAr0 == pozeAr1 && pozeAr1 == pozeAr2 && ifBet == false)     
        {    ifBet = true;
             std::cout << "dai la muie responsabil" << std::endl;
             belite += bet[betAr] * multi[pozeAr0];
             cateBeliteAm;
             cateBeliteAm = "Ai Atatea Belite: ";
             cateBeliteAm += std::to_string(belite);
             textChips =  TTF_RenderText_Solid(fontChips, cateBeliteAm.c_str(), {223, 194, 123});
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

    SDL_Rect pac1 ;
        pac1.h = 200;
        pac1.w = 200;
        pac1.x = 420;
        pac1.y = 550;  

    SDL_Rect pac2 ;
        pac2.h = 200;
        pac2.w = 200;
        pac2.x = 940;
        pac2.y = 550; 

    SDL_Rect pac3 ;
        pac3.h = 200;
        pac3.w = 200;
        pac3.x = 1380;
        pac3.y = 550;         

if(fread(clipRawOut, 1, 4 * 640 * 360, clipOutput) == 0)
{
    fseek(clipOutput, 921600 * 45, SEEK_SET);
}
SDL_DestroyTexture(textureClip);
SDL_FreeSurface(framesClipInfo);
framesClipInfo = SDL_CreateRGBSurfaceFrom(clipRawOut, 640, 360, 32, 2560, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
textureClip = SDL_CreateTextureFromSurface(randat, framesClipInfo);

    if(fread(clipPacRawOut1, 1, 4 * 374 * 112, clipOutPac1) == 0)
{
    fseek(clipOutPac1, 167552 * 0, SEEK_SET);
}
SDL_DestroyTexture(texClipPac1);
SDL_FreeSurface(clipPac1);
clipPac1 = SDL_CreateRGBSurfaceFrom(clipPacRawOut1, 374, 112, 32 , 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
texClipPac1 = SDL_CreateTextureFromSurface(randat, clipPac1);

    if(fread(clipPacRawOut2, 1, 4 * 374 * 112, clipOutPac2) == 0)
{
    fseek(clipOutPac2, 167552 * 0, SEEK_SET);
}
SDL_DestroyTexture(texClipPac2);
SDL_FreeSurface(clipPac2);
clipPac2 = SDL_CreateRGBSurfaceFrom(clipPacRawOut2, 374, 112, 32 , 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
texClipPac2 = SDL_CreateTextureFromSurface(randat, clipPac2);

    if(fread(clipPacRawOut3, 1, 4 * 374 * 112, clipOutPac3) == 0)
{
    fseek(clipOutPac3, 167552 * 0, SEEK_SET);
}
SDL_DestroyTexture(texClipPac3);
SDL_FreeSurface(clipPac3);
clipPac3 = SDL_CreateRGBSurfaceFrom(clipPacRawOut3, 374, 112, 32 , 4 * 374, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
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
if(std::chrono::duration_cast<std::chrono::milliseconds>(timepointEnter12 - timepointEnter1).count() < 1000)
{
    SDL_RenderCopy(randat, texClipPac1, NULL, &pac1);
}
if(std::chrono::duration_cast<std::chrono::seconds>(timepointEnter23 - timepointEnter2).count() < 2)
{
    SDL_RenderCopy(randat, texClipPac2, NULL, &pac2);
}
if(std::chrono::duration_cast<std::chrono::milliseconds>(timepointEnter34 - timepointEnter3).count() < 3000)
{
    SDL_RenderCopy(randat, texClipPac3, NULL, &pac3);
}
SDL_RenderPresent(randat);
startTimeF = endTimeF;
endTimeF = SDL_GetTicks();

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