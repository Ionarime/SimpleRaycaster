#include <SDL2/SDL.h>
#include <list>
#include <experimental/random>
struct Line{
int x1;int y1;int x2;int y2;};
struct col{
int r; int g; int b; int a;};
struct Linewc{
int x1;int y1;int x2;int y2; col c;};
struct point{
int x; int y;};
struct Sprite{
double x; double y; int type; int lastsize;};
struct dist{
double distance; int index;};
struct valSp{
double distance; int spriteHeight; int drawStartX; bool draw; int type; col c;};
struct ds{double distance1; double distance2;};
struct cir{int radiusSquared; int x; int y;};
struct crs{int lenght; int x; int y; int thicc;};
#define numSprites 2
Sprite sprite[numSprites] = {{0,0,1,0},{17,17,5,0}};
ds debs[numSprites];
dist iwtd[numSprites];
std::list<crs> crosses;
std::list<cir> circles;
std::list<point> points;
std::list<Line> lines;
std::list<col> couleur;
std::list<valSp> validSprites;
std::list<valSp> vvs;
std::list<Linewc> latelines;
#define mapWidth 60
#define mapHeight mapWidth
int screenWidth =640*1;
int screenHeight =480*1;
#define texWidth 64
#define texHeight 64
SDL_Surface* scr; //the single SDL surface used
Uint8* inkeys = 0;
SDL_Event event = {0};
bool up=false;
bool down=false;
bool right=false;
bool left=false;
bool space=false;
//Map #1
int worldMap[mapWidth][mapHeight];
//Colors:
col Magenta = {255,0,219,255}; //Valve magenta no textures
col RED = {255,50,50,255};
col Hred = {255,0,0,255};
col Red3p5 = {192,0,0,255};
col Mred = {128,0,0,255};
col Hbrown = {100,20,20,255};
col Mbrown = {50,10,10,255};
col GREEN = {50,255,50,255};
col Hgreen = {0,255,0,255};
col Green3p5 = {0,192,0,255};
col Mgreen = {0,128,0,255};
col BLUE = {50,50,255,255};
col Hblue = {0,0,255,255};
col Blue3p5 = {0,0,192,255};
col Mblue = {0,0,128,255};
col White = {255,255,255,255};
col Black = {0,0,0,255};
col Grey0p25 = {8,8,8,255};
col Grey0p5 = {16,16,16,255};
col Grey1 = {32,32,32,255};
col Grey2 = {64,64,64,255};
col Grey3 = {128,128,128,255};
col Grey3p5 = {192,192,192,255};
col Grey3p75 = {224,224,224,255};
col HudBlue = {60,210,220,255};
col SkyBlue = {35,130,230,255};
col Grey1p5 = {48,48,48,255};
struct mw{
int x; int y; bool isHorizontal; int type;};
#define MazeWallLenght 3
#define MAZE_WIDTH (mapWidth/MazeWallLenght-1)
#define MAZE_HEIGHT (mapHeight/MazeWallLenght-1)
struct wc{
bool h; bool w; bool checkedh; bool checkedw; int group;};
struct inher{
bool valid; int group;};
wc Maze[MAZE_WIDTH][MAZE_WIDTH];
void drawWall(int x, int y, bool isHorizontal, int group){
    if(!isHorizontal) Maze[x][y]= {true, Maze[x][y].w, true, Maze[x][y].checkedw, group};
    else Maze[x][y]= {Maze[x][y].h, true, Maze[x][y].checkedh, true, group};
    for(int i = 0; i < MazeWallLenght;i++){
        if(isHorizontal){worldMap[x*MazeWallLenght+i][y*MazeWallLenght]=group;}
        else{worldMap[x*MazeWallLenght][y*MazeWallLenght+i]=group;}
    }
}
inher validcheck(int x, int y, bool h){
    //Mazes are nicely walkable
    int ts=0;
    int group=0;
    if((Maze[x][y].w && h) || (Maze[x][y].h && !h) || (Maze[x-1][y].w && y!=0) || (Maze[x][y-1].h && x!=0)){
            ts++;
            if((Maze[x][y].w && h) || (Maze[x][y].h && !h)){group=Maze[x][y].group;}
            else if(Maze[x-1][y].w){group=Maze[x-1][y].group;}
            else{group=Maze[x][y-1].group;}
    }
    if(h && (Maze[x][y+1].h || Maze[x][y+1].w || Maze[x-1][y+1].w)){
            ts++;
            if(Maze[x][y+1].h || Maze[x][y+1].w){group=Maze[x][y+1].group;}
            else{group=Maze[x-1][y+1].group;}
    }
    else if(!h && (Maze[x+1][y-1].h || Maze[x+1][y].w || Maze[x+1][y].h)){
            ts++;
            if(Maze[x+1][y].w || Maze[x+1][y].h){group=Maze[x+1][y].group;}
            else{group=Maze[x+1][y-1].group;}
    }
    if(ts==1){return {true,group};}
    return {false,group};
}
mw chooseWall(){
    int num=std::experimental::randint(1, 2);
    bool isHorizontal = (num%2==0) ? false : true;
    int x = std::experimental::randint(1, MAZE_HEIGHT-2);
    int y = std::experimental::randint(1, MAZE_HEIGHT-2);
    return mw {x,y,isHorizontal,2};
}
mw chooseRoot(){
    int num=std::experimental::randint(1, 2);
    bool isHorizontal = (num%2==0) ? false : true;
    int x=std::experimental::randint(MAZE_WIDTH/4,3*MAZE_WIDTH/4);
    int y=std::experimental::randint(MAZE_HEIGHT/4,3*MAZE_HEIGHT/4);
    return mw {x,y,isHorizontal,2};
}
mw chooseTrunkv2(){
    int x=0,y=0;
    bool w=false;
    bool nworks=true;
    int runs = 0;
    while(nworks && runs<10){
    int side = std::experimental::randint(0,3);
    switch(side){
    case 0://up
        {
        x = std::experimental::randint(1,MAZE_WIDTH-1);
        y = 0;
        w = false;
        if(x!=1 && x!=MAZE_WIDTH-1){
            nworks=false;
        }
        if((x==1 && !Maze[x-1][y+1].w)||(x==MAZE_WIDTH-1 && !Maze[x][y+1].w)){
                nworks=false;
        }
        break;
        }
    case 1://down
        {
        x = std::experimental::randint(1,MAZE_WIDTH-1);
        y = MAZE_HEIGHT-1;
        w = false;
        if(x!=1 && x!=MAZE_WIDTH-1){
            nworks=false;
        }
        if ((x==1 && !Maze[x-1][y].w)||(x==MAZE_WIDTH-1 && !(Maze[x][y].w))){
                nworks=false;
        }
        break;
        }
    case 2://left
        {
        x = 0;
        y = std::experimental::randint(1,MAZE_HEIGHT-1);
        w = true;
        if(y!=MAZE_HEIGHT-1 && y!=1){
            nworks=false;
        }
        if((y==MAZE_HEIGHT-1 && !Maze[x+1][y].h)||(y==1 && !Maze[x+1][y-1].h)){
            nworks=false;
        }
        break;
        }
    case 3://right
        {
        x = MAZE_WIDTH-1;
        y = std::experimental::randint(1,MAZE_HEIGHT-1);
        w = true;
        if(y!=MAZE_HEIGHT-1 && y!=1){
            nworks=false;
        }
        if((y==MAZE_HEIGHT-1 && !(Maze[x][y].h)) || (y==1 && (!Maze[x][y-1].h))){
            nworks=false;
        }
        break;
        }
    }runs++;
    }if (nworks) return(mw {x,y,w,0});
    return(mw {x,y,w,1});
}

void mapGen(){
    int x = std::experimental::randint(1+5, MAZE_HEIGHT-2);
    int y = std::experimental::randint(1+5, MAZE_HEIGHT-2);
    sprite[numSprites-1]={(x-1.49)*MazeWallLenght,(y-1.49)*MazeWallLenght,5,0};
    for (int x=0; x<MAZE_WIDTH; x++){
    for (int y=0; y<MAZE_HEIGHT; y++){
        Maze[x][y].w=false;
        Maze[x][y].h=false;
    }}for(int x=0; x<mapWidth; x++){
    for(int y=0; y<mapHeight; y++){
        worldMap[x][y]=0;
    }}
    for (int x = 0; x < MAZE_WIDTH; x++){
        drawWall(x, MAZE_HEIGHT, true, 1);
        drawWall(x, 0, true, 1);}
    for (int y = 0; y < MAZE_HEIGHT; y++){
        drawWall(MAZE_WIDTH, y, false, 1);
        drawWall(0, y, false, 1);}
    for (int x = 0; x<mapWidth-2; x++){
        //worldMap[x][mapWidth-1]=1;
        worldMap[x][0]=1;
    }
    for (int y = 0; y<mapHeight-2; y++){
        //worldMap[mapHeight-1][y]=1;
        worldMap[0][y]=1;
    }
    for (int i = 0; i < 20;i++){
        if(i%5==0){mw rootbeer = chooseRoot();
        drawWall(rootbeer.x,rootbeer.y,rootbeer.isHorizontal,std::experimental::randint(1, 4));}
        mw cw=chooseTrunkv2();
        if(cw.type>0) drawWall(cw.x,cw.y,cw.isHorizontal,std::experimental::randint(1, 4));
    }
    for(int i=0; i<4096; i++){
        mw cw=chooseWall();
        inher v = validcheck(cw.x,cw.y,!cw.isHorizontal);
        if(v.valid) drawWall(cw.x,cw.y,cw.isHorizontal,v.group);
    }
}
//out of bounds map, allows this by not blocking access to going hither
void oobm(){
    for (int x=0; x<MAZE_WIDTH; x++){
    for (int y=0; y<MAZE_HEIGHT; y++){
    Maze[x][y].w=false;
    Maze[x][y].h=false;
    }}for(int x=0; x<mapWidth; x++){
    for(int y=0; y<mapHeight; y++){
        worldMap[x][y]=0;
    }}
    for (int x=1; x<MAZE_WIDTH; x+=2){
    for (int y=1; y<MAZE_HEIGHT; y+=2){
    drawWall(x,y,true,std::experimental::randint(1,4));
    drawWall(x,y,false,std::experimental::randint(1,4));}}
}
//line with color, shorter way to line and color push
void lwc(int x1, int y1, int x2, int y2, col c){
    lines.push_back(Line {x1,y1,x2,y2});
    couleur.push_back(c);
}
//point with color, same thing for points
void pwc(int x, int y, col c){
    points.push_back(point {x, y});
    couleur.push_back(c);
}
//fake rectangle color
void frc(int x1, int y1, int x2, int y2, col c){
    for(int i=y1; i!=y2; i++){
        lwc(x1,i,x2,i,c);
    }
}
col darken(col c, int amount){
    col nc;
    nc.r = (c.r-amount > 0) ? c.r-amount : 0;
    nc.g = (c.g-amount > 0) ? c.g-amount : 0;
    nc.b = (c.b-amount > 0) ? c.b-amount : 0;
    return(nc);
}
/*
bool soundinit = false;
SDL_AudioSpec wavSpec;
Uint32 wavLength;
Uint8 *wavBuffer;
int played=0;
void playsound(){
    if(!soundinit){
        SDL_Init(SDL_INIT_AUDIO);
        soundinit=true;
    }
    //"Sound effects and music are fundamental in giving life to a game. Here we are going to play a simple WAV file using just the native SDL2 Audio APIs.
    //Unfortunately these APIs are very tricky to use, and the documentation is littered with incoherent examples and legacy function calls.
    //For this reason, most people prefer to use the SDL_mixer extension library to handle sound and music."
    //TLDR: SDL2 Audio manager sucks so hard you can only play WAVs and this breaks after like 13 uses
    played++;
    // load WAV file
    SDL_LoadWAV("moog.wav", &wavSpec, &wavBuffer, &wavLength);
    // open audio device
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    // play audio
    int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
    if(played==5){
        // clean up
        played=0;
        SDL_Init(SDL_INIT_AUDIO);
    }
}
*/
double bdirX=0.2;
double bdirY=0.8;
double oldX=0;
double oldY=0;
void emov(){
    double ss=0.05;
    for(int i = 0; i<numSprites; i++){
        switch(sprite[i].type){
        case 1:
            {
            double m=0.1;

            //!Version 2.6 Bouncy Balls lightweight
            //After discovering a grave error in the rendering system

            int nextposx = (int) (sprite[i].x + (bdirX * m));
            int nextposy = (int) (sprite[i].y + (bdirY * m));
            int x = sprite[i].x;
            int y = sprite[i].y;
            if(worldMap[nextposx][nextposy]>0){
            if(worldMap[nextposx][y]>0){
                    bdirX= -1*bdirX;//playsound();
            }else if(worldMap[x][nextposy]>0){
                    bdirY= -1*bdirY;//playsound();
            }}
            sprite[i].x=sprite[i].x+bdirX*m;
            sprite[i].y=sprite[i].y+bdirY*m;
            break;

            /**
            //!Version 2.5 Bouncy Balls
            //slightly shorter logic
            //added next next nextpos for preemptive bouncing
            //-->Less disgusting wall clipping

            //(not very effective at negating such unwanted behavior
            //but eh, looks more complicated and should make a difference
            //like now the ball sometimes uses quantum physics and
            //just passes through a wall (pretty cool eh?))

            int nextposx= int(sprite[i].x+(bdirX*m));
            int nextposy= int(sprite[i].y+(bdirY*m));
            int nextposx2= int(sprite[i].x+(bdirX*m*2));
            int nextposy2= int(sprite[i].y+(bdirY*m*2));
            int nextposx3= int(sprite[i].x+(bdirX*m*5));
            int nextposy3= int(sprite[i].y+(bdirY*m*5));
            int X = sprite[i].x;
            int Y = sprite[i].y;
            if((worldMap[nextposx][nextposy]>0 || worldMap[nextposx2][nextposy2]>0 || worldMap[nextposx3][nextposy3]>0)
                && (worldMap[nextposx][Y]>0 || worldMap[nextposx2][nextposy]>0)){
                    bdirX= -1*bdirX;playsound();
            }else if((worldMap[nextposx][nextposy]>0 ||worldMap[nextposx2][nextposy2]>0 || worldMap[nextposx3][nextposy3]>0)
                && (worldMap[X][nextposy]>0 || worldMap[nextposx][nextposy2]>0)){
                    bdirY= -1*bdirY;playsound();
            }
            sprite[i].x=sprite[i].x+bdirX*m;
            sprite[i].y=sprite[i].y+bdirY*m;
            **/
            break;
            }
        case 2:
            {
            int spriteX = sprite[i].x;
            int spriteY = sprite[i].y;
            int c4 = rand()%4;
            switch(c4){
                case 0:
                    if(worldMap[spriteX+1][spriteY]==0) sprite[i].x+=ss;
                    break;
                case 1:
                    if(worldMap[spriteX-1][spriteY]==0) sprite[i].x-=ss;
                    break;
                case 2:
                    if(worldMap[spriteX][spriteY+1]==0) sprite[i].y+=ss;
                    break;
                case 3:
                    if(worldMap[spriteX][spriteY-1]==0) sprite[i].y-=ss;
                    break;
            }break;
            }
        case 3:
            if (sprite[i].x<20 && sprite[i].y>=20){sprite[i].x+=ss;}
            else if(sprite[i].x>=20 && sprite[i].y>15){sprite[i].y-=ss;}
            else if(sprite[i].x>15 && sprite[i].y<=15){sprite[i].x-=ss;}
            else if(sprite[i].x<=15&&sprite[i].y<20){sprite[i].y+=ss;}
            break;
        }
    }
}
bool SimpleGraphics = true;
void HeadUpDisplay(int x, int y, double dx, double dy){
    //int pointsize=screenHeight/160;
    int pointsize = 2;
    //screenHeight ought to be greater than mapHeight+10
    int yOffset=screenHeight-(mapHeight*pointsize)-10;
    int xOffset=10;
    lwc(xOffset+pointsize*(x+0.5),yOffset+pointsize*(y+0.5),xOffset+pointsize*(x+1.5*dx+0.5),yOffset+pointsize*(y+1.5*dy+0.5),GREEN);
    bool d=false;
    for(int i = 0; i<mapWidth*pointsize; i+=pointsize){
        for(int j = 0; j<mapHeight*pointsize; j+=pointsize){
            col c2pb = Black;
            d=false;
            if(i==x*pointsize&&j==y*pointsize){c2pb=GREEN;d=true;}
            for(int ee = 0; ee<numSprites; ee++){
                int spriteX = sprite[ee].x;
                int spriteY = sprite[ee].y;
                if(i==spriteX*pointsize && j==spriteY*pointsize){
                    if(!d){c2pb = Magenta;d=true;break;}
                }
            }
            if(!d && (worldMap[i/pointsize][j/pointsize]!=0)){
                    d=true;
                    col curcol;
                    switch(worldMap[i/pointsize][j/pointsize])
                        {
                        case 1:
                            curcol=Hred;
                            break;
                        case 2:
                            curcol=Hgreen;
                            break;
                        case 3:
                            curcol=Hblue;
                            break;
                        case 4:
                            curcol=Grey3p75;
                    break;
                        default:
                            curcol=Magenta;
                            break;
                        }c2pb=curcol;
            }
            if(d){
            for(int px = 0; px!=pointsize; px++){
                for(int py = 0; py!=pointsize; py++){
                    pwc(i+px+xOffset, yOffset+py+j, c2pb);
                    }
                }
            }
        }
    }
}
void PseudoSegmentFont(double num, int xOffset, int yOffset, int sll = 6){
    //why this and not SDL_ttf.h?
    //because I can
    //no SDL_ttf.dll needed ;3
    col bc=Black;
    col fg=White;
    int space=sll/2;
    bool tl=false;
    int l=0;
    char const * nc = std::to_string(num).c_str();
    while (nc[l] != '\0') l++;
    for(int i=0; i<(sll+space-1)*2;i++){
        lwc(xOffset-1,yOffset-1+i,xOffset+(sll+space)*l,yOffset-1+i,bc);
    }
    for(int n = 0; n<l; n++){
        int xo=xOffset+n*(space+sll);
        tl=false;
        switch(n[nc]){
        case 46:
            lwc(xo,yOffset, xo+sll,yOffset+sll*2,fg);
            break;
        case 48: //0
            lwc(xo,yOffset,xo+sll,yOffset,fg);
            lwc(xo,yOffset,xo,yOffset+sll*2,fg);
            lwc(xo,yOffset+sll*2,xo+sll,yOffset+sll*2,fg);
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll*2,fg);
            break;
        case 49: //1
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll*2,fg);
            break;
        case 50: //2
            tl=true;
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll,fg);
            lwc(xo,yOffset+sll,xo,yOffset+sll*2,fg);
            break;
        case 51: //3
            tl=true;
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll*2,fg);
            break;
        case 52: //4
            lwc(xo,yOffset,xo,yOffset+sll,fg);
            lwc(xo,yOffset+sll,xo+sll,yOffset+sll,fg);
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll*2,fg);
            break;
        case 53: //5
            tl=true;
            lwc(xo,yOffset,xo,yOffset+sll,fg);
            lwc(xo+sll,yOffset+sll,xo+sll,yOffset+sll*2,fg);
            break;
        case 54: //6
            tl=true;
            lwc(xo,yOffset,xo,yOffset+sll*2,fg);
            lwc(xo+sll,yOffset+sll,xo+sll,yOffset+sll*2,fg);
            break;
        case 55: //7
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll*2,fg);
            lwc(xo,yOffset,xo+sll,yOffset,fg);
            break;
        case 56: //8
            tl=true;
            lwc(xo,yOffset,xo,yOffset+sll*2,fg);
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll*2,fg);
            break;
        case 57: //9
            tl=true;
            lwc(xo,yOffset,xo,yOffset+sll,fg);
            lwc(xo+sll,yOffset,xo+sll,yOffset+sll*2,fg);
            break;
        default:
            lwc(xo,yOffset+sll,xo+sll,yOffset+sll,fg);
            break;
        }if(tl){
            lwc(xo,yOffset,xo+sll,yOffset,fg);
            lwc(xo,yOffset+sll,xo+sll,yOffset+sll,fg);
            lwc(xo,yOffset+sll*2,xo+sll,yOffset+sll*2,fg);
        }
    }
}
void circle(int radiusSquared, int ix, int iy, col c){
    bool draw=true;
    for (std::list<cir>::const_iterator i = circles.begin(); i != circles.end(); ++i)
        {
            cir c = *i;
            if(c.radiusSquared==radiusSquared && c.x == ix && c.y == iy){draw=false;}
        }
        if(draw){
        circles.push_back(cir {radiusSquared, ix, iy});
        int R=sqrt(radiusSquared);
        int x=0; int y=R; int d=5-4*R;
        int dA=12; int dB=20-8*R;
        while (x<y){
            ///filled quadrants
            lwc(x+ix,y+iy,y+ix,x+iy,c);
            lwc(x+ix,-y+iy,y+ix,-x+iy,c);
            lwc(-x+ix,y+iy,-y+ix,x+iy,c);
            lwc(-x+ix,-y+iy,-y+ix,-x+iy,c);
            if(d<0){
                d+=dA; dB+=8;
            }else{
            y--; d+=dB; dB+=16;}
            x++; dA+=8;


        }
        /*
        R=sqrt(radiusSquared)/2;
        x=0; y=R; d=5-4*R;
        dA=12; dB=20-8*R;
        while (x<y){
            ///filled quadrants
            lwc(x+ix,y+iy,y+ix,x+iy,c);
            lwc(x+ix,-y+iy,y+ix,-x+iy,c);
            lwc(-x+ix,y+iy,-y+ix,x+iy,c);
            lwc(-x+ix,-y+iy,-y+ix,-x+iy,c);
            if(d<0){
                d+=dA; dB+=8;
            }else{
            y--; d+=dB; dB+=16;}
            x++; dA+=8;
        }*/
        /*
        for(int i = -radius; i<radius+0.5; i++){
            lwc(x,y,x+i,y+sqrt(radiusSquared-i*i),c);
            lwc(x,y,x-i,y-sqrt(radiusSquared-i*i),c);
            lwc(x,y,x+sqrt(radiusSquared-i*i),y+i,c);
            lwc(x,y,x-sqrt(radiusSquared-i*i),y-i,c);
            }
        */}
    }
void cross(int lenght, int x, int y, int thicc, col c){
    bool draw=true;
    for (std::list<crs>::const_iterator i = crosses.begin(); i != crosses.end(); ++i)
        {
            crs c = *i;
            if(c.lenght==lenght && c.x == x && c.y == y && c.thicc == thicc){draw=false;}
        }
    if(draw){
        crosses.push_back(crs {lenght, x, y, thicc});
        frc(x-lenght,y-thicc,x+lenght,y+thicc, c);
        frc(x-thicc,y-lenght,x+thicc,y+lenght, c);
    }
}
void flag(int Size, int x, int y, col c){
    Size = sqrt(Size);
    Size = (Size>screenHeight) ? screenHeight : Size;
    for(int i = -Size*0.4; i<0; i++){
        lwc(x+i,y-Size*0.2,x+i,y+Size*Size,Grey1p5);
    }frc(x-Size*0.4,y-Size*0.2,x+Size,y+Size*0.7,RED);
}
int main(int argc, char ** argv)
{
    // variables
    bool quit = false;
    bool debug = false;
    bool nightmode = false;
    bool showMap = false;
    bool ac=false;
    bool aac=false;
    bool info = false;
    SDL_Event event;
    double posX = 1, posY = 1;  //x and y start position
    double dirX = 0, dirY = 1; //initial direction vector
    double planeX = 0.66, planeY = 0; //the 2d raycaster version of camera plane
    double oldfps=1234567890;
    double time = 0; //time of current frame
    double oldTime = 0; //time of previous frame
    int uc=0;
    // init SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Lupus et Lapis",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
    SDL_RenderClear(renderer);
    int w = screenWidth; int h = screenHeight;
    // Create a slider
    SDL_Rect slider = {0, screenHeight/2, screenWidth, 20};
    int sliderValue = 0;
    int sliderMaxValue = 150;
    bool dragging = false;
    SDL_Surface* image2 = SDL_LoadBMP("set.bmp");//106x32
    SDL_Texture* textureHarm = SDL_CreateTextureFromSurface(renderer, image2);
    SDL_Rect pain;
    pain.x = screenWidth/2-106;
    pain.y = screenHeight/2-32*2;
    pain.w = 106*2;
    pain.h = 32*2;
    SDL_SetWindowPosition(window, 10, 10);
    bool knife = true;
    while(knife){
        SDL_Delay(10);
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
                return 1;
                break;
        case SDL_KEYDOWN:
            if(SDLK_RETURN==event.key.keysym.sym) knife=false;
            break;
        case SDL_MOUSEBUTTONDOWN:
                // Start dragging
                dragging = true;
            break;
        case SDL_MOUSEBUTTONUP:
            // Stop dragging
            dragging = false;
            break;
        case SDL_MOUSEMOTION:
            if (dragging)
            {
                // Calculate the new slider value
                sliderValue = ((event.motion.x - slider.x) * sliderMaxValue) / slider.w;
                if (sliderValue < 0) sliderValue = 0;
                if (sliderValue > sliderMaxValue) sliderValue = sliderMaxValue;
            }
            break;
        }
        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the slider
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &slider);
        SDL_Rect sliderValueRect = {slider.x, slider.y, (slider.w * sliderValue) / sliderMaxValue, slider.h};
        SDL_RenderFillRect(renderer, &sliderValueRect);

        // Update the renderer
        SDL_RenderCopy(renderer, textureHarm, NULL, &pain);
        double factor = sliderValue*0.01+1;
        w = screenWidth*factor;
        h = screenHeight*factor;
        SDL_SetWindowSize(window, w, h);
        SDL_RenderPresent(renderer);
    }SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
    SDL_RenderClear(renderer);
    screenWidth=w;
    screenHeight=h;

    SDL_Surface* infoimg = SDL_LoadBMP("ii.bmp");//182x147
    SDL_Texture* infotxt = SDL_CreateTextureFromSurface(renderer, infoimg);
    SDL_Rect Moonlit;
    Moonlit.x = (screenWidth-182*2)/2;
    Moonlit.y = (screenHeight-147*2)/2;
    Moonlit.w = 182*2;
    Moonlit.h = 147*2;

    SDL_Surface* image = SDL_LoadBMP("lupus et lapis.bmp"); //112x28
    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, image);
    SDL_Rect dest;
    dest.x = screenWidth/2-112;
    dest.y = screenHeight/2-28;
    dest.w = 112*2;
    dest.h = 28*2;

    SDL_Surface* image2077 = SDL_LoadBMP("LSI2.bmp"); //182x50
    SDL_Texture* texture2077 = SDL_CreateTextureFromSurface(renderer, image2077);
    SDL_Rect helium;
    helium.x = screenWidth/2-182/2;
    helium.y = screenHeight/2+28;
    helium.w = 182;
    helium.h = 50;

    bool cs = false;
    int ssy=2;
    int c1 = 0;
    bool cc = false;
    int ccc = 0;
    bool cccc=false;
    int c5=10;
    bool ins=true;
    double povx=0;
    double povy=0;
    while(ins){
        SDL_PollEvent(&event);
        switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                ins = false;
                break;
            case SDL_KEYDOWN:
                ins = false;
                break;
            }
        for(int x=0;x<screenWidth;x+=3){
            int c = rand() % 255 + 1;
            int cc = rand()%3;
            for(int y=0;y<screenHeight/2;y+=ssy){
                if(c-(y/ssy)>=0) switch(cc){
                    case 0: for(int s=0; s<ssy; s++){
                        pwc(x,screenHeight-y-s,col {c-(y/ssy),0,0,255});
                        pwc(x,y+s,col {0,0,c-(y/ssy),255});}break;
                    case 1: for(int s=0; s<ssy; s++){
                        pwc(x,screenHeight-y-s,col {c-(y/ssy),32/((c-(y/ssy)+1)),32/((c-(y/ssy)+1)),255});
                        pwc(x,y+s,col {128/((c-(y/ssy)+1)),12/((c-(y/ssy)+1)),c-(y/ssy),255});}break;
                    case 2: for(int s=0; s<ssy; s++){
                        pwc(x,screenHeight-y-s,col {c-(y/ssy),12/((c-(y/ssy)+1)),169/((c-(y/ssy)+1)),255});
                        pwc(x,y+s,col {32/((c-(y/ssy)+1)),32/((c-(y/ssy)+1)),c-(y/ssy),255});}break;
                }
            }
        }
        c1++;
        if(c1==400){cccc=true;c1=0;}
        if(cccc && c1==c5){
            c1=0;
            if(!cc){
                ccc++;
                SDL_SetRenderDrawColor(renderer, ccc,ccc,ccc, 255);
                }
            else{
                ccc--;
                SDL_SetRenderDrawColor(renderer, ccc,ccc,ccc, 255);
                }
            if(ccc==40){
                SDL_SetRenderDrawColor(renderer, 255,255,255,50);
                }
            if(ccc==50){
                cc=true;
            }else if(ccc==40){
                if(c5>4){c5--;cc=false;cccc=false;c1=400-c5;}
                else{c5=10;ccc=0;cc=false;cccc=false;SDL_SetRenderDrawColor(renderer, ccc,ccc,ccc, 255);
                }
            }
            SDL_RenderClear(renderer);
        }

        std::list<col>::iterator c = couleur.begin();
        for (std::list<point>::const_iterator i = points.begin(); i != points.end(); ++i)
        {
            col cll = *c;
            SDL_SetRenderDrawColor(renderer, cll.r, cll.g, cll.b, cll.a);
            point p = *i;
            SDL_RenderDrawPoint(renderer, p.x, p.y);
            std::advance(c,1);
        }
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderDrawLine(renderer,0,screenHeight/2,screenWidth,screenHeight/2);
        couleur.clear();points.clear();
        SDL_RenderPresent(renderer);
        SDL_RenderCopy(renderer, texture1, NULL, &dest);
        SDL_RenderCopy(renderer, texture2077, NULL, &helium);
        SDL_RenderPresent(renderer);
        if(!cs){
            cs=true;
            mapGen();
        }
    }couleur.clear();points.clear();SDL_SetRenderDrawColor(renderer, 0,0,0, 255);SDL_RenderClear(renderer);
    int sc=0;
    while(!quit){
            //sprite calculations
            validSprites.clear();
            vvs.clear();
            circles.clear();
            crosses.clear();
            //sort sprites far to close
            for(int i=0; i < numSprites; i++){
                iwtd[i] = {((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)),i};
            }
            ac=true;
            while(ac){
                aac=false;
                for(int i = 0; i<numSprites-1; i++){
                    if(iwtd[i].distance>iwtd[i+1].distance){
                            dist temp = i[iwtd];
                            i[iwtd] = iwtd[i + 1];
                            iwtd[i + 1] = temp;
                            aac=true;
                    }
                }
                ac=aac;
            }
            for(int i = 0; i<numSprites; i++){
                double sizeOFLeSoufle=0.1;
                if(sprite[i[iwtd].index].type!=0){
                    //Transform the sprite relative to camera
                    double spriteX = sprite[i[iwtd].index].x - posX;
                    double spriteY = sprite[i[iwtd].index].y - posY;

                    //transform sprite with the inverse camera matrix
                    // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
                    // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
                    // [ planeY   dirY ]                                          [ -planeY  planeX ]

                    double invDet = 1.0 / (planeX * dirY - dirX * planeY); //required for correct matrix multiplication

                    double transformY = invDet * (-planeY * spriteX + planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D
                    if(transformY>0){
                    double transformX = invDet * (dirY * spriteX - dirX * spriteY);
                    int spriteScreenX = int((w / 2) * (1 + transformX / transformY));
                    int PP = abs(int(h / (transformY)))*abs(int(h / (transformY)))*sizeOFLeSoufle*sizeOFLeSoufle;
                    if (spriteScreenX<w && spriteScreenX>0) validSprites.push_back(valSp {sqrt(iwtd[i].distance), PP, spriteScreenX, false, i[iwtd].index, Grey0p25});
            }}}
        for(int x = 0; x < w; x++){
            //for out of def stuff
            bool s = false;
            //calculate ray position and direction
            double cameraX = 2 * x / (double)w - 1; //x-coordinate in camera space
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;
            if(x==w/2){povx=rayDirX;povy=rayDirY;}
            //which box of the map we're in
            int mapX = int(posX);
            int mapY = int(posY);
            //length of ray from current position to next x or y-side
            double sideDistX;
            double sideDistY;
            //length of ray from one x or y-side to next x or y-side
            //these are derived as:
            //deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
            //deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
            //which can be simplified to abs(|rayDir| / rayDirX) and abs(|rayDir| / rayDirY)
            //where |rayDir| is the length of the vector (rayDirX, rayDirY). Its length,
            //unlike (dirX, dirY) is not 1, however this does not matter, only the
            //ratio between deltaDistX and deltaDistY matters, due to the way the DDA
            //stepping further below works. So the values can be computed as below.
            // Division through zero is prevented, even though technically that's not
            // needed in C++ with IEEE 754 floating point values.
            double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
            //condition ? value_if_true : value_if_false
            //In essence, this line of code is calculating the distance that a ray would need to travel in the x direction
            //in order to move one unit in the y direction, assuming the ray is traveling in the direction given by the
            //vector (rayDirX, rayDirY). If rayDirX is 0, then the ray is traveling parallel to the y axis and there is no
            //x distance to consider, hence the very large number being used as a placeholder.
            double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
            double perpWallDist;
            //what direction to step in x or y-direction (either +1 or -1)
            int stepX;
            int stepY;
            int hit = 0; //was there a wall hit?
            int side; //was a NS or a EW wall hit?
            //calculate step and initial sideDist
            if(rayDirX < 0){
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }else{
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }if(rayDirY < 0){
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }else{
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }
            //perform DDA (Digital differential analyzer)
            while(hit == 0)
            {
            //jump to next map square, either in x-direction, or in y-direction
                if(sideDistX < sideDistY){
                  sideDistX += deltaDistX;
                  mapX += stepX;
                  side = 0;
                }else{
                  sideDistY += deltaDistY;
                  mapY += stepY;
                  side = 1;
                }if(worldMap[mapX][mapY] > 0) hit = 1;
            }
            //calculate distance projected on camera direction. this is the shortest distance from the point where the wall is
            //hit to the camera plane. Euclidean to center camera point would give fisheye effect!
            //this can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
            //for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
            //because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
            //steps, but we subtract deltaDist once because one step more into the wall was taken above.
            if(side == 0) perpWallDist = (sideDistX - deltaDistX);
            else          perpWallDist = (sideDistY - deltaDistY);
            //calculate height of line to draw on screen
            int lineHeight = (int)(h / perpWallDist);
            //calculate lowest and highest pixel to fill in current stripe
            int drawStart = -lineHeight / 2 + h / 2;
            if(drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + h / 2;
            if(drawEnd >= h) drawEnd = h - 1;
            col curcol;
            if(SimpleGraphics&&!nightmode){
                if (side == 1){
                switch(worldMap[mapX][mapY])
                {
                    case 1:
                        curcol=Mred;
                        break;
                    case 2:
                        curcol=Mgreen;
                        break;
                    case 3:
                        curcol=Mblue;
                        break;
                    case 4:
                        curcol=Grey3;
                        break;
                    default:
                        curcol=Magenta; //Valve
                        //out of def. stuff
                        s = true;
                        break;
                }
                }else{
                    switch(worldMap[mapX][mapY])
                    {
                    case 1:
                        curcol=Red3p5;
                        break;
                    case 2:
                        curcol=Green3p5;
                        break;
                    case 3:
                        curcol=Blue3p5;
                        break;
                    case 4:
                        curcol=Grey3p5;
                        break;
                    default:
                        s=true;
                        curcol=Magenta;
                        break;
                    }
                }
                }else if (nightmode){
                    switch(worldMap[mapX][mapY])
                {
                case 1:
                    curcol=Mred;
                    break;
                case 2:
                    curcol=Mgreen;
                    break;
                case 3:
                    curcol=Mblue;
                    break;
                case 4:
                    curcol=Grey3;
                    break;
                default:
                    curcol=Magenta;
                    s = true;
                    break;
                }
                }
                curcol = nightmode ? darken(curcol,perpWallDist*(mapHeight/2)) : darken(curcol,perpWallDist*(mapHeight/50));
                if(!s) lwc(x, drawStart, x, drawEnd, curcol);
                else{
                    sc++;
                    if(sc%2==0){
                        lwc(x,drawStart,x,h/2, Black);
                        lwc(x,drawEnd,x,h/2, Magenta);
                    }else{
                        lwc(x,drawStart,x,h/2, Magenta);
                        lwc(x,drawEnd,x,h/2, Black);
                    }
                }
                for (std::list<valSp>::const_iterator i = validSprites.begin(); i != validSprites.end(); ++i){
                    valSp check = *i;
                    if(check.drawStartX==x && debug){debs[check.type] = {check.distance,perpWallDist};}
                    if(check.distance<perpWallDist
                       && check.drawStartX-sqrt(check.spriteHeight)/4>=0
                       && check.drawStartX+sqrt(check.spriteHeight)/4<=w){
                        vvs.push_back(valSp {check.distance, check.spriteHeight, check.drawStartX, true, check.type, check.c});
                    }else{
                        if(check.distance>perpWallDist
                            && check.drawStartX+sqrt(check.spriteHeight)+1>x
                            && check.drawStartX-sqrt(check.spriteHeight)-1<x){
                            bool drawIt=true;
                            for (std::list<valSp>::const_iterator j = validSprites.begin(); j != validSprites.end(); ++j){
                                valSp chung = *j;
                                if(chung.distance<perpWallDist
                                    && chung.drawStartX+sqrt(chung.spriteHeight)+1>x
                                    && chung.drawStartX-sqrt(chung.spriteHeight)-1<x){
                                    drawIt=false;break;}
                            }if(drawIt){latelines.push_back(Linewc {x, drawStart, x, drawEnd, curcol});}
                        }
                    }
                }
            }
            for (std::list<valSp>::const_iterator i = vvs.begin(); i != vvs.end(); ++i)
            {
                valSp check = *i;
                sprite[check.type].lastsize=sqrt(check.spriteHeight);
                col curcol = check.c;
                if(check.draw){
                    switch(sprite[check.type].type){
                    case 4:
                        cross(sqrt(check.spriteHeight),check.drawStartX,screenHeight/2,sqrt(check.spriteHeight)-1,nightmode ? darken(curcol,check.distance*(mapHeight/2)) : darken(curcol,check.distance*(mapHeight/50)));
                        break;
                    case 5:
                        circle(check.spriteHeight, check.drawStartX, screenHeight/2,nightmode ? darken(curcol,check.distance*(mapHeight/2)) : darken(curcol,check.distance*(mapHeight/50)));
                        break;
                    default:
                        circle(check.spriteHeight, check.drawStartX, screenHeight/2,nightmode ? darken(RED,check.distance*(mapHeight/2)) : darken(RED,check.distance*(mapHeight/50)));
                        break;
                    }
                }
            }
            for(std::list<Linewc>::const_iterator i = latelines.begin(); i != latelines.end(); ++i){
                Linewc x = *i;
                if(debug){lwc(x.x1,x.y1,x.x2,x.y2,Magenta);}
                else{lwc(x.x1,x.y1,x.x2,x.y2,x.c);}
            }
            //timing for input and FPS counter
            oldTime = time;
            time = SDL_GetTicks();
            double frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
            double fps=(1.0 / frameTime);
            uc++; //updates fps every 20 frames so you can read it
            if(uc==20){oldfps=fps;uc=0;}
            emov();
            if(debug){
                PseudoSegmentFont(oldfps,5,5);
                for(int i=0; i<numSprites+1;i++){
                    if (i<numSprites){
                    PseudoSegmentFont(debs[i].distance1,5,23+(i*36));PseudoSegmentFont(debs[i].distance2,168,23+(i*36));
                    PseudoSegmentFont(sprite[i].x,5,41+(i*36));PseudoSegmentFont(sprite[i].y,168,41+(i*36));}
                    else if (i==numSprites){
                            PseudoSegmentFont(dirX,5,23+(i*36));PseudoSegmentFont(dirY,168,23+(i*36));
                            PseudoSegmentFont(planeX,5,41+(i*36));PseudoSegmentFont(planeY,168,41+(i*36));
                    }
                }
            }
            //speed modifiers
            double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
            double rotSpeed = frameTime * 3.0; //the constant value is in radians/second
            SDL_Delay(10);
            SDL_PollEvent(&event);
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if(SDLK_c==event.key.keysym.sym){
                    debug= debug ? false : true;
                    showMap=showMap?false:true;
                }if(SDLK_g==event.key.keysym.sym){
                    mapGen();
                }if(SDLK_n==event.key.keysym.sym){
                    nightmode = (nightmode) ? false : true;
                }if(SDLK_SPACE==event.key.keysym.sym){
                    bdirX=povx;
                    bdirY=povy;
                    for(int i = 0; i<numSprites; i++){
                        if(sprite[i].type==1){
                            sprite[i].x=posX;
                            sprite[i].y=posY;
                        }
                    }
                    space=true;
                }if(SDLK_w==event.key.keysym.sym){
                    up=true;
                }if (SDLK_s==event.key.keysym.sym){
                    down=true;
                }if (SDLK_d==event.key.keysym.sym){
                    right=true;
                }if (SDLK_a==event.key.keysym.sym){
                    left=true;
                }if (SDLK_i==event.key.keysym.sym){
                    info=info?false:true;
                }if (SDLK_o==event.key.keysym.sym){
                oobm();}
                break;
            case SDL_KEYUP:
                if(SDLK_w==event.key.keysym.sym){
                    up=false;
                }if (SDLK_s==event.key.keysym.sym){
                    down=false;
                }if (SDLK_d==event.key.keysym.sym){
                    right=false;
                }if (SDLK_a==event.key.keysym.sym){
                    left=false;
                }if(SDLK_SPACE==event.key.keysym.sym){
                    space=false;
                }
                break;
            }
            if(up){
                if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
                if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
            }if (down){
                if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
                if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
            }if (right){
                double oldDirX = dirX;
                dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
                dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
                planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
            }if (left){
                double oldDirX = dirX;
                dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
                dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
                planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
            }
        //new map check//found check
        if(int(posX)==int(sprite[numSprites-1].x) && int(posY)==int(sprite[numSprites-1].y)){
            posX = 1, posY = 1;  //x and y start position
            dirX = 0, dirY = 1; //initial direction vector
            planeX = 0.66, planeY = 0; //the 2d raycaster version of camera plane
            mapGen();
        }
        //floor and sky
        if(SimpleGraphics && !nightmode){
            SDL_SetRenderDrawColor(renderer, 50,50,50, 255);
            SDL_RenderClear(renderer);
            col rc=SkyBlue;
            SDL_SetRenderDrawColor(renderer, rc.r, rc.g, rc.b, rc.a);
            for(int i=0;i<h/2;i++){
            SDL_RenderDrawLine(renderer, 0, i, w, i);}
            /* Not needed, Floor = Background color
            rc=Grey3;
            SDL_SetRenderDrawColor(renderer, rc.r, rc.g, rc.b, rc.a);
            for(int i=0;i<h/2;i++){
                    SDL_RenderDrawLine(renderer, 0, i, w, i);*/
        }else{
            SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
            SDL_RenderClear(renderer);
        }
        if(showMap) HeadUpDisplay(posX,posY,povx,povy);
        std::list<col>::iterator c = couleur.begin();
        for (std::list<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
        {
            col cll = *c;
            SDL_SetRenderDrawColor(renderer, cll.r, cll.g, cll.b, cll.a);
            Line line = *i;
            SDL_RenderDrawLine(renderer, line.x1, line.y1, line.x2, line.y2);
            std::advance(c,1);
        }
        for (std::list<point>::const_iterator i = points.begin(); i != points.end(); ++i)
        {
            col cll = *c;
            SDL_SetRenderDrawColor(renderer, cll.r, cll.g, cll.b, cll.a);
            point p = *i;
            SDL_RenderDrawPoint(renderer, p.x, p.y);
            std::advance(c,1);
        }
        lines.clear();couleur.clear();points.clear();latelines.clear();
        if(info){
            SDL_RenderCopy(renderer, infotxt, NULL, &Moonlit);
        }
        // render window
        SDL_RenderPresent(renderer);
    }
    // cleanup SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
