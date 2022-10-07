#include<SDL2/SDL.h>
#include<SDL2/sdl_image.h>
#include<GL/glu.h>

#include<iostream>
#include<cstdlib>
#include<vector>
#include<time.h>
#include<algorithm>

//#include<string>

using namespace std;

int selectedCardIndex = -1;
int cardSelectedCount = 0;
int setResetTime=4000;

class Clock {
    Uint32 minimum_fps_delta_time = (1000 / 6);
    Uint32 last_game_step = SDL_GetTicks();
    uint32_t last_tick_time = 0;
public:
    uint32_t delta = 0;
    void ticK() {
        Uint32 now = SDL_GetTicks();

        // Check so we don't render for no reason, avoid having a 0 delta time
        if (last_game_step < now) {

            Uint32 delta_time = now - last_game_step;

            if (delta_time > minimum_fps_delta_time) {
                delta_time = minimum_fps_delta_time; // slow down if the computer is too slow
            }
            delta = delta_time;

            last_game_step = now;
        }
    }
};

class Card {
    float angle = 0;
    GLuint frontTexture;
    GLuint backTexture;

    float sizeX,sizeY;

    float animeTime = 35;
    float countAnimeTime = 0;

    float resetTime = setResetTime;
    float countResetTime = 2;

    bool fliping = false;
    bool front = true;
    string asset;

    bool isMatch = false;

public:
    float x, y;

    Card(string path, float x, float y,float sizeX,float sizeY) {
        this->x = x;
        this->y = y;
        this->sizeX = sizeX;
        this->sizeY = sizeY;
        this->asset = path;
        countAnimeTime = animeTime;
        this->countResetTime=-1;

        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            cout << "error, nie mozna zaladowac tesktury"<<endl;
            return;
        }
        glGenTextures(1, &backTexture);
        glBindTexture(GL_TEXTURE_2D, backTexture);
        int Mode = GL_RGB;
        switch (surface->format->BytesPerPixel)
        {
        case 4:
            if (surface->format->Rmask == 0x000000ff)
                Mode = GL_RGBA;
            else
                Mode = GL_RGBA; //GL_BGRA;
            break;
        case 3:
            if (surface->format->Rmask == 0x000000ff)
                Mode = GL_RGB;
            else
                Mode = GL_RGBA;// GL_BGR;
            break;
        default:
            cout << "Error, image is not truecolor." << endl;
            return;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);
        SDL_FreeSurface(surface);

        surface = IMG_Load("assets/front.jpg");
        if (!surface) {
            cout << "Error, nie mozna zaladowac tesktury"<<endl;
            return;
        }
        glGenTextures(1, &frontTexture);
        glBindTexture(GL_TEXTURE_2D, frontTexture);
        Mode = GL_RGB;
        switch (surface->format->BytesPerPixel)
        {
        case 4:
            if (surface->format->Rmask == 0x000000ff)
                Mode = GL_RGBA;
            else
                Mode = GL_RGBA; //GL_BGRA;
            break;
        case 3:
            if (surface->format->Rmask == 0x000000ff)
                Mode = GL_RGB;
            else
                Mode = GL_RGBA;// GL_BGR;
            break;
        default:
            std::cout << "Error, image is not truecolor." << std::endl;
            return;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);
        SDL_FreeSurface(surface);
    }

    bool onMouseClick(float mouseX, float mouseY) {
        if (cardSelectedCount >= 2) {
            return false;
        }
        if (!front) {
            return false;
        }
        if (isMatch) {
            return false;
        }

        if (mouseX > (x - sizeX) and mouseX < (x + sizeX) and mouseY > (y - sizeY) and mouseY < (y + sizeY)) {
            fliping  = true;
            return true;
        }
        return false;
    }

    /*float getX() {
        return x;
    }

    float getY() {
        return y;
    }

    void setX(float arg) {
        this->x = arg;
    }

    void setY(float arg) {
        this->y = arg;
    }*/

    void setMatched() {
        isMatch = true;
    }

    bool isMatched() {
        return isMatch;
    }

    bool isCardMatched(Card cmCard) {
        return asset.compare(cmCard.asset) == 0;
    }
    string getAsset() {
        return asset;
    }

    void update(float deltaTime) {
        if (fliping) {
            countAnimeTime -= deltaTime;
            if (countAnimeTime <= 0) {
                countAnimeTime = animeTime;
                angle += 10;

                if (angle >= 100) {
                    fliping = false;
                    angle = 0;

                    front = !front;
                }
            }
        }

        if (!isMatch) {
            countResetTime -= deltaTime;
            if (countResetTime <= 0) {
                countResetTime = resetTime;
                if (!front) {
                    fliping = true;
                }
                selectedCardIndex = -1;
                cardSelectedCount = 0;
            }
        }
    }

    void draw() {
        if (front) {
            glBindTexture(GL_TEXTURE_2D, frontTexture);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, backTexture);
        }
        glEnable(GL_TEXTURE_2D);
        glPushMatrix();

        glTranslatef(x, y, 0);
        glRotatef(angle, 0, 1, 0);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex2f(-sizeX, sizeY);
        glTexCoord2f(1.0, 1.0);
        glVertex2f(sizeX, sizeY);
        glTexCoord2f(1.0, 0.0);
        glVertex2f(sizeX, -sizeY);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(-sizeX, -sizeY);
        glEnd();
        glPopMatrix();
    }
};

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 480.0 / 480.0, 1, 500.0);
    glLoadIdentity();
    glOrtho(0, 480, 480, 0, 100, -100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
}

string getAsset(int i)
{
    string assets[18] = {
       "assets/1.jpg",
       "assets/2.jpg",
       "assets/3.jpg",
       "assets/4.jpg",
       "assets/5.jpg",
       "assets/6.jpg",
       "assets/7.jpg",
       "assets/8.jpg",
       "assets/9.jpg",
       "assets/10.jpg",
       "assets/11.jpg",
       "assets/12.jpg",
       "assets/13.jpg",
       "assets/14.jpg",
       "assets/15.jpg",
       "assets/16.jpg",
       "assets/17.jpg",
       "assets/18.jpg"
    };

    return assets[i];
}

int main(int argc, char* args[])
{
    srand(time(0));
    //SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Gra Memory Konrad Stepniak Jan Janiszewski",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        480, 480,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glcontext);

    Clock clock;

    vector<Card> cards;
    int rows, cols;

    while (true) {
        cout << "Podaj liczbe wierszy: ";
        cin >> rows;
        cout << "Podaj liczbe kolumn: ";
        cin >> cols;
        if ((rows * cols) % 2 != 0) {
            cout << "Nieparzysta liczba kart!\Musisz podac wiersze i kolumny ktorych iloczyn jest liczba parzysta"<<endl;
        }
        else {
            break;
        }
    }
    cout<<endl<<"Gra wystartowala!\nPrzejdz do okna gry."<<endl;

    float sizeX, sizeY, startX, startY, center;
    if(rows == cols)
    {
        sizeX = 480 / cols / 2;
        sizeY = 480 / rows / 2;

        startX = sizeX;
        startY = sizeY;
    }
    else
    {
        if (rows>cols)
        {
            sizeX = 480 / rows / 2;
            sizeY = 480 / rows / 2;

            center = (480 - (rows*sizeX))/4;
            startX = sizeX + center;
            startY = sizeY;
        } else
        {
            sizeX = 480 / cols / 2;
            sizeY = 480 / cols / 2;

            center = (480 - (cols*sizeY))/4;
            startX = sizeX;
            startY = sizeY + center;
        }
    }

    sizeX = sizeX * 0.95;
    sizeY = sizeY * 0.95;

    //startX = startX * 1.1;
    //startY = startY * 1.1;
    float marginX = sizeX + (sizeX * 0.1);
    float marginY = sizeY + (sizeY * 0.1);

    float posX = startX;
    float posY = startY;

    int assetIndex=0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            string asset = getAsset(assetIndex);
            assetIndex++;
            if (assetIndex >= 18 || assetIndex >= (rows*cols)/2)
                assetIndex=0;

            Card card(asset, posX, posY, sizeX, sizeY);
            cards.push_back(card);
            posX += sizeX + marginX;
        }
        posX = startX;
        posY += sizeY + marginY;
    }

    int random;
    float tempX, tempY;
    for (int i=0; i<cards.size(); i++)
    {
        random = rand() % (cards.size()-1);
        //cout<<"rand: "<<random<<endl;

        swap(cards[random].x, cards[i].x);
        swap(cards[random].y, cards[i].y);

        /*temp2 = cards[i].y;
        cards[i].y = cards[cards.size()-i].y;
        cards[cards.size()-i].y = temp2;*/
    }

    //cards[1].setX(cards[1].getX()+100);

    int loop = 1;
    SDL_Event event;
    init();
    while (loop == 1)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                loop = 0;
                break;

            case SDL_KEYDOWN:
                break;
            case SDL_MOUSEBUTTONDOWN:
                for (int i = 0; i < cards.size(); i++) {
                    if (cardSelectedCount < 2)
                    {
                        if (cards[i].onMouseClick(event.button.x, event.button.y)) {
                            cardSelectedCount++;
                            if (selectedCardIndex != -1 && selectedCardIndex != i) {
                                if (cards[i].isCardMatched(cards[selectedCardIndex])) {
                                    cards[i].setMatched();
                                    cards[selectedCardIndex].setMatched();
                                    selectedCardIndex = -1;
                                    if (cardSelectedCount>=2) cardSelectedCount = 0;
                                    //cards[i].countResetTime = 1;

                                    int cardMatched = 0;
                                    for (Card card : cards) {
                                        if (card.isMatched()) {
                                            cardMatched++;
                                        }
                                    }
                                    if (cardMatched == cards.size()) {
                                        cout << "Wygrales!";
                                    }
                                }
                            }
                            else {
                                selectedCardIndex = i;
                            }
                        }
                    }
                }
                break;
            }
        }
        display();

        clock.ticK();

        for (int i = 0; i < cards.size();i++) {
            cards[i].update(clock.delta);
            cards[i].draw();
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
