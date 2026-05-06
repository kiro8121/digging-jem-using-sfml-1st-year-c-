 #include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <random>
#include <vector>

using namespace sf;
using namespace std;

//================ particel for explosion ( Madonna and kiro  ) =================
// ده جزءصغير من الانفجار الواحد
struct Particle
{
    // بعرف مكانه
    float x, y;
    // بعرف اتجاهه
    float vx, vy;
    // بعرف الوقت اللي هو فيه دلوقتي
    float current_lifetime;
    // بعرف الوقت كلو اللي المفروض بعديه يخلص الانفجار وبستخدمه في النسبه
    float max_lifetime;
    // حجم الجزء الصغير اوي من الانفجار الواحد
    float size;
    sf::Color color;
    // عشان اقول يبقا في انفجاراصلا ولا
    bool active;
};

//================ particel system ( Madonna and kiro  ) =================
// ده بعرف عددالاجزاء الصغيره اوي في كل انفجار واحد
const int MAX_PARTICLES = 300;
// وده عدد الانفجارات كلها الكبيره
const int MAX_EXPLOSIONS = 20;
// وده انجار واحد عشان استخدمه في العشرين
struct Explosion
{
    // بستخدم الداتا للاجزاء الصغيره كلها عشان تبان طبيعيه
    Particle particles[MAX_PARTICLES];
    // ده بيعرفني انا استخدمت كامparticle فعليا من 300
    int count;
    bool active;
    // دي بتخليني اعيد الانفجار من الاول وببتدي ب ان مفيش اي انفجار
    void reset()
    {
        count = 0;
        active = true;
    }
    // دي بقا اللي مسئوله انها تعمل الانفجار نفسه
    // particle count ده معناه انا عايزه استخدم كام particle
    void create(float x, float y, sf::Color baseColor, int particle_count)
    {
        // ببتدي طبعا من ان مفيش لسه اي حته ابتدت في الانفجار ومستخدمتش part لسه
        count = 0;
        // همشي في اللوب لغايه 300 مينفعش اعديها
        for (int i = 0; i < particle_count && i < MAX_PARTICLES; i++)
        {
            // ref var p بياخد كل قيمه من particles بتغير index ويحجز الميموري بتاعت كل واحد في p عشان يساوو بعض
            Particle &p = particles[count++];
            // بخلي يختارلي زاويه عشوائيه  بعد كدا احولها راديان
            float angle = (rand() % 360) * 3.14159f / 180.f;
            // وهكذا سرعه كل partبخلي السرعات مخلفه بين 100 ل 299 تبان حلوهه
            float speed = 100 + rand() % 200;
            // هنا بعرف لكل particle مكانه فين
            p.x = x;
            p.y = y;
            // بحسب السرعه لكل part كاتجاه
            p.vx = cos(angle) * speed;
            p.vy = sin(angle) * speed;
            // هنا part لسه في الاول
            p.current_lifetime = 1.0f;
            // بعد ما الوقت ده يخلص الانفجار هيختفي
            p.max_lifetime = 1.0f;
            // بخلي الحجم مختلف برضو
            p.size = 3 + (rand() % 4);
            // بقيس كل لون نسبته وبخليها مختلفه عشان تبقا رايقه وفي كل لون ميقلش عن 0ولا يزيد عن 255
            int r = std::max(0, std::min(255, baseColor.r + (rand() % 40 - 20)));
            int g = std::max(0, std::min(255, baseColor.g + (rand() % 40 - 20)));
            int b = std::max(0, std::min(255, baseColor.b + (rand() % 40 - 20)));
            // وبظبط درجات الالوان التلاته
            p.color = sf::Color(r, g, b);
            p.active = true;
        }
    }
    //////function (Kirollos and Madonna)
    // ودي اللي بتخلي يبان ان في حركه للانفجار وبعدل علي كل فريم
    void update(float dt)
    {
        const float gravity = 400.f;
        const float friction = 0.98f;
        //  هعدي علي عدد ال parts اللي هستخمهم
        for (int i = 0; i < count; i++)
        {
            // وبقعد ازود علي حسب count
            Particle &p = particles[i];
            // لو الانفجار وقف هفكس واكمل
            if (!p.active)
                continue;
            // هشوف الوقت الانا فيه ومع الوقت هفضل اقلله عشان الانفجار يختفي تدريجي
            p.current_lifetime -= dt;
            // خلاص الوقت خلص
            if (p.current_lifetime <= 0)
            {
                p.active = false;
                continue;
            }
            // الجازبيه بتزودلي السرعه لتحت
            p.vy += gravity * dt;
            // السرعه بتقل بتدريج
            p.vx *= friction;
            p.vy *= friction;
            // وفي الاخر بحسب مكانه بالسرعات الاخيره مع الوقت
            p.x += p.vx * dt;
            p.y += p.vy * dt;
        }
    }
    // هتخلي الانفجار يبانلي علي الشاشه
    void draw(sf::RenderWindow &w)
    {
        // علي شكل دايره
        sf::CircleShape shape;

        for (int i = 0; i < count; i++)
        {
            Particle &p = particles[i];
            if (!p.active)
                continue;
            // عشان اعرف هيبقا موجود بنسبه قد ويبقا موجود حجمه قد ايه
            float existance_rate = p.current_lifetime / p.max_lifetime;
            // عشان الشكل يصغر تدريجي لغايه ما يختفي ودرجته تقل برضو
            shape.setRadius(p.size * existance_rate);
            shape.setFillColor(sf::Color(
                p.color.r,
                p.color.g,
                p.color.b,
                (sf::Uint8)(255 * existance_rate)));
            // بحط مكانها الاخير ومدوره
            shape.setPosition(p.x, p.y);
            // برسم علي الشاشه
            w.draw(shape);
        }
    }
    // تخلي الجزء اللي خلص ةقته يخلص والباقي يكمل عادي
    bool isFinished()
    {
        for (int i = 0; i < count; i++)
            if (particles[i].active)
                return false;

        return true;
    }
};
// ده اللي بيخلي في انفجار لما البلاير ياخد جوهره او يموت
struct ExplosionManager
{
    // بمشي بقا علي العشرين انفجار
    Explosion explosions[MAX_EXPLOSIONS];
    //  ببتدي من الزيرو عدد الانفجارات
    int count = 0;

    void addExplosion(float x, float y, sf::Color color, int particle_count)
    {
        // لو اللي هستخدمه اكبر من20 هرجع ومش هستخدمه
        if (count >= MAX_EXPLOSIONS)
            return;
        // واكمل عادي الباقي
        explosions[count].create(x, y, color, particle_count);
        count++;
    }
    // لماياخد جوهرهيحصل انفجار في مكان البلاير
    void addDiamondExplosion(float x, float y)
    {
        // بقسم درجات الاول ومكان الانفجار علي كل شويهparticles
        addExplosion(x, y, sf::Color(0, 255, 255), 20);
        addExplosion(x, y, sf::Color(255, 255, 0), 15);
        addExplosion(x, y, sf::Color(255, 255, 255), 10);
    }
    // وهنا لما يموت يحصل انفجار في مكان البلاير لما يموت
    void addDeathExplosion(float x, float y)
    {
        addExplosion(x, y, sf::Color(255, 0, 0), 25);
        addExplosion(x, y, sf::Color(255, 120, 0), 20);
    }
    // بعدل في كل فريم مع الزمن
    void update(float dt)
    {
        // بعدي علي كل انفجار من العشرين
        for (int i = 0; i < count; i++)
            explosions[i].update(dt);

        for (int i = 0; i < count;)
        {
            // الj ده الانفجار اللي خلص فبشيلو وبحط ال explosion اللي بعده بداله
            if (explosions[i].isFinished())
            {
                // لازم انقص واحد هنا عشان لو اخرelement هيطلع بره الarray
                for (int j = i; j < count - 1; j++)
                    explosions[j] = explosions[j + 1];

                count--;
            }
            else
                i++;
        }
    }

    void draw(sf::RenderWindow &w)
    {
        for (int i = 0; i < count; i++)
            explosions[i].draw(w);
    }

    void clear()
    {
        count = 0;
    }
};

//================ (Marcellino) =================

// enum highlights the idea of that the computer works by 1 and 0
// (1 -------> working) || (0 -------> Not working)
enum Gamestate
{
    menu,
    playing,
    instructions_state,
    game_level,
    options,
    Normal_version,
    Comic_version
};
Gamestate currentstate = menu;

//================ Constant(kiro ashraf and kiro khaled) =================
const int cell_size = 40;
const int cols = 32;
const int rows = 22;

const int empty_type = 0;
const int wall_type = 1;
const int sand_type = 2;
const int stone_type = 3;
const int diamond_type = 4;

const int state_playing = 0;
const int state_dead = 1;
const int state_win = 2;

//================ struct (kirollos Ashraf)=================
struct Entity
{
    int grid_x, grid_y;
    float visual_x, visual_y;
    float target_x, target_y;
    float move_speed;
    bool moving;
};

//================ scoreboard (MARCELLINO) =================
//================ STL (Data structure) 25% ====================
void score_board_initialization(unordered_map<string, int> &GAMEDATA)
{
    GAMEDATA.insert({"Collected", 0});
    GAMEDATA.insert({"Points", 0});
    GAMEDATA.insert({"Time", 0});
}

void increasing_collected(unordered_map<string, int> &GAMEDATA)
{
    auto itr = GAMEDATA.find("Collected");
    if (itr != GAMEDATA.end())
    {
        if (GAMEDATA["Collected"])
        {
            itr->second--;
        }
    }
}

void increasing_points(unordered_map<string, int> &GAMEDATA)
{
    auto itr = GAMEDATA.find("Points");
    if (itr != GAMEDATA.end())
    {
        itr->second += 5;
    }
}

void decreasing_time(unordered_map<string, int> &GAMEDATA)
{
    auto itr = GAMEDATA.find("Time");
    if (itr != GAMEDATA.end())
    {
        if (GAMEDATA["Time"] > 0)
        { // -----> الشرط دا لضمان ان لما العداد يوصل لصفر ميعدش بالسالب
            itr->second--;
        }
    }
}

void display_options(RenderWindow &window, Sprite &OPBg, Sprite &arrowPOINTER, int place_of_elgwhara,
                     Sprite &ON_photo, Sprite &OFF_Photo, Music &bgMusic, Sprite &volumee,
                     Sprite &cursorr, Sprite &no_volumee)
{
    window.draw(OPBg); // window ---> elshasha ely bt3rd el7aga || draw ---> hwa command elrasm 3la elshasha
    if (bgMusic.getStatus() == Music::Playing)
    {
        window.draw(ON_photo);
        window.draw(volumee);
        window.draw(cursorr);
    }
    else
    {
        window.draw(OFF_Photo);
        window.draw(no_volumee);
    }
    float tzbeeet_scales_elgwhara_elybtshawer_3la_eloption[5][2] = {
        {265.f, 150.f}, // audio on/off
        {260.f, 225.f}, // audio volume
        {260.f, 290.f}, // Normal version
        {260.f, 352.f}, // comic version
        {260.f, 419.f}  // quit to main menu
    };

    arrowPOINTER.setPosition(tzbeeet_scales_elgwhara_elybtshawer_3la_eloption[place_of_elgwhara][0],
                             tzbeeet_scales_elgwhara_elybtshawer_3la_eloption[place_of_elgwhara][1]);

    // [0] ----> carries values of X
    // [1] ----> carries values of y
    // ex : when the programm compile [0] ---> will carry 265.f and put it in the place of x in setposition
    window.draw(arrowPOINTER);
}

void ON_OFF(Texture &TEX, Sprite &photo, const string &PHOTO_YOU_WANT, float x, float y, float scale)
{
    TEX.loadFromFile(PHOTO_YOU_WANT);
    photo.setTexture(TEX);
    photo.setPosition(x, y);
    photo.setScale(scale, scale);
}

void audio_control(Texture &TEX, Sprite &photo, const string &PHOTO_YOU_WANT, float x, float y, float scale)
{
    TEX.loadFromFile(PHOTO_YOU_WANT);
    photo.setTexture(TEX);
    photo.setPosition(x, y);
    photo.setScale(scale, scale);
}

float audio_volume_UP_and_DOWN(float position_of_the_cursor, float minX, float maxX)
{
    float range = maxX - minX;
    float current_volume = position_of_the_cursor - minX;
    float percentage = current_volume / range;
    return percentage * 100.f;
}

//================ MAIN FUNCTION (kiro Khaled // kiro Ashraf // donna // maro // dani // mohra //mohra )=================
int main()
{
    int window_width = cols * cell_size;
    int window_height = rows * cell_size;

    RenderWindow Window(VideoMode(window_width, window_height), "Digging Jim - Complete Edition with Explosions");
    Window.setFramerateLimit(60);

    //================ EXPLOSION MANAGER (Donna & Kiro khaled) =================
    ExplosionManager explosionManager;

    //================ SCOREBOARD(MARCELLINO) =================
    unordered_map<string, int> gamedata;
    score_board_initialization(gamedata);

    //================ TEXTURES =================

    // Try to load textures (dani and ashraf)- use fallback colors if files don't exist
    Texture player_tex, wall_tex, stone_tex, sand_tex, diamond_tex, door_tex;

    // Try to load textures(dani ) - use fallback colors if files don't exist
    bool texturesLoaded = true;
    if (!door_tex.loadFromFile("door.png"))
        texturesLoaded = false;
    if (!player_tex.loadFromFile("CHARA.jpeg"))
        texturesLoaded = false;
    if (!wall_tex.loadFromFile("BLOCK_OF_IRON.gif"))
        texturesLoaded = false;
    if (!stone_tex.loadFromFile("ZALATA.jpeg"))
        texturesLoaded = false;
    if (!sand_tex.loadFromFile("SANDDD.jpeg"))
        texturesLoaded = false;
    if (!diamond_tex.loadFromFile("GEM.jpeg"))
        texturesLoaded = false;

    //================ sprites (dani and kiro ashraf ) =================
    Sprite player_spr, wall_spr, stone_spr, sand_spr, diamond_spr, door_spr;

    if (texturesLoaded)
    {
        player_spr.setTexture(player_tex);
        wall_spr.setTexture(wall_tex);
        stone_spr.setTexture(stone_tex);
        sand_spr.setTexture(sand_tex);
        diamond_spr.setTexture(diamond_tex);
        door_spr.setTexture(door_tex);

        // Scale each sprite individually
        player_spr.setScale(
            (float)cell_size / player_spr.getGlobalBounds().width,
            (float)cell_size / player_spr.getGlobalBounds().height);

        wall_spr.setScale(
            (float)cell_size / wall_spr.getGlobalBounds().width,
            (float)cell_size / wall_spr.getGlobalBounds().height);

        stone_spr.setScale(
            (float)cell_size / stone_spr.getGlobalBounds().width,
            (float)cell_size / stone_spr.getGlobalBounds().height);

        sand_spr.setScale(
            (float)cell_size / sand_spr.getGlobalBounds().width,
            (float)cell_size / sand_spr.getGlobalBounds().height);

        diamond_spr.setScale(
            (float)cell_size / diamond_spr.getGlobalBounds().width,
            (float)cell_size / diamond_spr.getGlobalBounds().height);

        door_spr.setScale(
            (float)cell_size / door_spr.getGlobalBounds().width,
            (float)cell_size / door_spr.getGlobalBounds().height);
    }

    //================ kiro ashraf  =================
    bool facing_right = true;
    float base_scale_x = texturesLoaded ? (float)cell_size / player_tex.getSize().x : 1.0f;
    float base_scale_y = texturesLoaded ? (float)cell_size / player_tex.getSize().y : 1.0f;

    //================ FONT =================
    Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
    {
        font.loadFromFile("/System/Library/Fonts/Helvetica.ttc"); // macOS fallback
    }

    Text game_over_text, win_text, diamonds_text;

    game_over_text.setFont(font);
    game_over_text.setString("GAME OVER! Press R to restart ");
    game_over_text.setCharacterSize(50);
    game_over_text.setFillColor(Color::Red);
    game_over_text.setPosition(window_width / 2 - 320, window_height / 2 - 30);

    win_text.setFont(font);
    win_text.setString("YOU WIN! Press R to restart ");
    win_text.setCharacterSize(50);
    win_text.setFillColor(Color::Yellow);
    win_text.setPosition(window_width / 2 - 280, window_height / 2 - 30);

    diamonds_text.setFont(font);
    diamonds_text.setCharacterSize(28);
    diamonds_text.setFillColor(Color::Cyan);
    diamonds_text.setPosition(10, 10);

    //================ moving names (  Mohra Boulos   ) =================

    string devNames = "Developed by: KIROLLOS KHALED - KIROLLOS ASHRAF - MOHRAEL MAGDY - MADONNA MAGDY - MOHRAEL BOLOUS - DANIEL MINA - MARCELLINO WADIE";
    Text devText;
    devText.setFont(font);
    devText.setString(devNames);
    devText.setCharacterSize(60);
    devText.setStyle(Text::Bold | Text::Italic);
    devText.setFillColor(Color(250, 212, 0));
    devText.setOutlineThickness(3.5f);
    devText.setOutlineColor(Color::Black);
    devText.setPosition(window_width, window_height - 150);
    float devSpeed = 90.f;
    float devWidth = devText.getGlobalBounds().width;

    //     //================ bars ( Marcellino ) =================
    RectangleShape uiBar(Vector2f(window_width, 60.f));
    uiBar.setFillColor(Color(25, 25, 25));
    uiBar.setOutlineThickness(-2);
    uiBar.setOutlineColor(Color(80, 80, 80));
    uiBar.setPosition(0, window_height - 60);

    Text uiText;
    uiText.setFont(font);
    uiText.setCharacterSize(40);
    uiText.setFillColor(Color(255, 215, 0));
    uiText.setOutlineThickness(6.5f);
    uiText.setOutlineColor(Color::Black);
    uiText.setPosition(25, window_height - 60);
    //================ menu graphics ( Marcellino )=================
    Texture menuu;
    Sprite menuBg;
    if (menuu.loadFromFile("photo.jpeg"))
    {
        menuBg.setTexture(menuu);
        float scaleX = (float)window_width / menuu.getSize().x;
        float scaleY = (float)window_height / menuu.getSize().y;
        menuBg.setScale(scaleX, scaleY);
    }

    Texture arrow;
    Sprite arrowPOINTER;
    if (arrow.loadFromFile("arrow-removebg-preview.png"))
    {
        arrowPOINTER.setTexture(arrow);
        arrowPOINTER.setScale(0.27f, 0.27f);
    }
    // دا بالظبط زي ارري تو دي كسؤول عن حركو الجوهرة او السهم في اربع اماكن علي محورين ال اكس و الواي
    Vector2f MARCELLINO[4] = {
        Vector2f(610.f, 270.f), // Start Game
        Vector2f(595.f, 340.f), // Load Caves
        Vector2f(632.f, 419.f), // Options
        Vector2f(670.f, 480.f)  // Quit
    };
    // دايما البلاير لما يفتح اللعبة هيلاقي الجوهرة واقفة عند اول اوبشن الي هو ستارت جيم

    // تطبيقا اهو للكلام الي فات ان ينفع استبدل فيكتور تو اف ب ارري تو دي و جربنا الموضوع دا بس المرادي علي شاشة الليفيل اوف جيم
    int the_place_at_which_arrow_stands = 0;

    float LEVEL_POSITIONS[3][2] = {
        {710.f, 360.f},
        {665.f, 444.f},
        {710.f, 525.f},
    };
    int Level_arrow_index = 0;

    //================ sound effects and audio (Marcellino) =================
    // نغمة المينيو الرءيسية
    Music yala_ya_yasser_sama3_3mo;
    if (yala_ya_yasser_sama3_3mo.openFromFile("SOUNDD.ogg"))
    {
        yala_ya_yasser_sama3_3mo.setLoop(true);
        yala_ya_yasser_sama3_3mo.play();
    }
    //================ gameplay music (Marcellino // Kirollos Ashraf) =================

    // دي النغمة الي بتشتغل اول ما تدخل تلعب فعليا

    Music gameplay_music;
    bool gameplay_music_loaded = gameplay_music.openFromFile("SOUND_EL_GAME.ogg");
    if (gameplay_music_loaded)
        gameplay_music.setLoop(true);
    //**********************************************************************************************
    Music ya_Fashel;
    // الدالة بتاعة اوبن فروم فايل التابعه ل سفمل بترجع صح لو لاقت الفايل فعلا يعني ترووو \\ لاكن في الشرط بقوله لو ملقتش (علامة تعجب) طلع ايرور
    if (!ya_Fashel.openFromFile("YA_FASHEL.ogg"))
    {
        cout << "Error loading death music\n";
    }
    // ****************************************************************************************
    Music ZAGHRATY_YA_7AGA;
    // الحاجة بتزغرط اول ما البلاير يكسب
    if (!ZAGHRATY_YA_7AGA.openFromFile("ZAGHRATY_YA_7AGA.ogg"))
    {
        cout << "Error loading za8rata music\n";
    }
    //*********************************************************************************************
    Music YALLAHWAY;

    if (!YALLAHWAY.openFromFile("YALAHWAAAY.ogg"))
    {
        cout << "Error loading latm music\n";
    }
    bool died_by_time = false;
    bool yalahway_played = false;
    // الفلاجز دي لضمان عدم حدوث اي تضارب في تشغيل الاصوات
    //*****************************************************************************************

    Music HWA_KOL_SHWAYA_A3EED;

    if (!HWA_KOL_SHWAYA_A3EED.openFromFile("hwa-kol-shwaya-e33ed.ogg"))
    {
        cout << "Error loading restart music\n";
    }
    //******************************************************************************************
    Music YALA_BENA;

    if (!YALA_BENA.openFromFile("yala-bena.ogg"))
    {
        cout << "Error loading yala bena music\n";
    }

    //================ option tab (Marcellino) =================
    Texture Options;
    Sprite OPBg;
    if (Options.loadFromFile("Options.jpeg"))
    {
        OPBg.setTexture(Options);
        float scaleX = (float)window_width / Options.getSize().x;
        float scaleY = (float)window_height / Options.getSize().y;
        OPBg.setScale(scaleX, scaleY);
    }
    //=======================instructions(Marcellino // Mohra Bolous // Mohra Magdy)===========================
    Texture instructions;
    Sprite INBG;
    if (instructions.loadFromFile("Instructions_photoo.jpeg"))
    {
        INBG.setTexture(instructions);
        float scaleX = (float)window_width / Options.getSize().x;
        float scaleY = (float)window_height / Options.getSize().y;
        INBG.setScale(scaleX, scaleY);
    }
    //=======================Game level screen( Mohra Magdy ) ====================
    Texture game_level_Tex;
    Sprite game_levelbg;
    if (game_level_Tex.loadFromFile("GAMELEVEL.jpeg"))
    {
        game_levelbg.setTexture(game_level_Tex);
        float scaleX = (float)window_width / game_level_Tex.getSize().x;
        float scaleY = (float)window_height / game_level_Tex.getSize().y;
        game_levelbg.setScale(scaleX, scaleY);
    }

    // ====================== Options Screen (Marcellino) ================================
    int place_of_elgwhara = 0;
    Texture ON, OFF, volume, cursor, no_volume;
    Sprite ON_photo, OFF_Photo, volumee, cursorr, no_volumee;
    ON_OFF(ON, ON_photo, "ONN-removebg-preview.png", 420.f, 150.f, 0.37f);
    ON_OFF(OFF, OFF_Photo, "OFFF-removebg-preview.png", 430.f, 160.f, 0.36f);
    audio_control(volume, volumee, "VOLUME-removebg-preview.png", 582.f, 180.f, 0.68f);
    audio_control(cursor, cursorr, "CURSOR-removebg-preview.png", 582.f, 207.f, 0.68f);
    audio_control(no_volume, no_volumee, "NO_SOUND-removebg-preview.png", 570.f, 82.f, 0.999f);

    const float minX = 419.f;
    const float maxX = 760.f;

    //================ map (Daniel) =================
    int map[rows][cols];
    float fall_offset[rows][cols];

    int total_diamonds = 0;
    int diamonds_left = 0;
    int game_state = state_playing;

    //================ reset map (Daniel) =================
    auto reset_game = [&](Entity &p)
    {
        // Clear all explosions (donna and Kiro khaled )
        explosionManager.clear();
        // dani
        for (int y = 0; y < rows; y++)
            for (int x = 0; x < cols; x++)
                fall_offset[y][x] = 0.f;

        for (int y = 0; y < rows; y++)
            for (int x = 0; x < cols; x++)
                map[y][x] = (x == 0 || x == cols - 1 || y == 0 || y >= rows - 2) ? wall_type : sand_type;
        // map[y][x] = (x == 0 || x == cols - 1 || y == 0 || y == rows - 1) ? wall_type : sand_type;
        // دا السطر بتاع دانيال انا بس عدلته عشان اللاعب كان بينزل ور السكور بورد و كمان عدلت ارقام بسيطة فالماب ديزاين عشان تتوافق مع التعديل داا

        //================ map restored and design (Daniel) =================
        ////////////wall_type
        map[0][0] = wall_type;
        map[0][1] = wall_type;
        map[0][2] = wall_type;
        map[0][3] = wall_type;
        map[0][4] = wall_type;
        map[0][5] = wall_type;
        map[0][6] = wall_type;
        map[0][7] = wall_type;
        map[0][8] = wall_type;
        map[0][9] = wall_type;
        map[0][10] = wall_type;
        map[0][11] = wall_type;
        map[0][12] = wall_type;
        map[0][13] = wall_type;
        map[0][14] = wall_type;
        map[0][15] = wall_type;
        map[0][16] = wall_type;
        map[0][17] = wall_type;
        map[0][18] = wall_type;
        map[0][19] = wall_type;
        map[0][20] = wall_type;
        map[0][21] = wall_type;
        map[0][22] = wall_type;
        map[0][23] = wall_type;
        map[0][24] = wall_type;
        map[0][25] = wall_type;
        map[0][26] = wall_type;
        map[0][27] = wall_type;
        map[0][28] = wall_type;
        map[0][29] = wall_type;
        map[0][30] = wall_type;
        map[0][31] = wall_type;
        map[1][0] = wall_type;
        map[2][0] = wall_type;
        map[3][0] = wall_type;
        map[4][0] = wall_type;
        map[5][0] = wall_type;
        map[6][0] = wall_type;
        map[7][0] = wall_type;
        map[8][0] = wall_type;
        map[9][0] = wall_type;
        map[10][0] = wall_type;
        map[11][0] = wall_type;
        map[12][0] = wall_type;
        map[13][0] = wall_type;
        map[14][0] = wall_type;
        map[15][0] = wall_type;
        map[16][0] = wall_type;
        map[17][0] = wall_type;
        map[18][0] = wall_type;
        map[19][0] = wall_type;
        map[20][0] = wall_type;
        map[21][0] = wall_type;
        map[13][1] = wall_type;
        map[13][2] = wall_type;
        map[13][3] = wall_type;
        map[13][4] = wall_type;
        map[13][5] = wall_type;
        map[13][6] = wall_type;
        map[13][7] = wall_type;
        map[13][8] = wall_type;
        map[13][9] = wall_type;
        map[13][10] = wall_type;
        map[13][11] = wall_type;
        map[13][12] = wall_type;
        map[13][13] = wall_type;
        map[13][18] = wall_type;
        map[13][19] = wall_type;
        map[13][20] = wall_type;
        map[13][21] = wall_type;
        map[13][22] = wall_type;
        map[13][23] = wall_type;
        map[13][24] = wall_type;
        map[13][25] = wall_type;
        map[13][26] = wall_type;
        map[13][27] = wall_type;
        map[13][28] = wall_type;
        map[13][29] = wall_type;
        map[13][30] = wall_type;
        map[13][31] = wall_type;

        // diamond_type (7)
        map[5][5] = diamond_type;
        map[11][16] = diamond_type;
        map[10][29] = diamond_type;
        map[18][22] = diamond_type;
        map[17][17] = diamond_type;
        map[1][31] = wall_type;
        map[7][7] = diamond_type;
        map[19][13] = diamond_type;
        map[1][7] = diamond_type;
        map[7][19] = diamond_type;
        map[15][5] = diamond_type;
        map[15][6] = diamond_type;
        map[1][30] = diamond_type;
        map[1][29] = diamond_type;
        map[2][4] = diamond_type;
        map[9][3] = diamond_type;
        map[17][29] = diamond_type;
        map[3][13] = diamond_type;
        map[4][14] = diamond_type;
        map[3][15] = diamond_type;
        map[4][16] = diamond_type;
        map[3][17] = diamond_type;
        map[4][18] = diamond_type;

        // stone_type
        map[1][3] = stone_type;
        map[1][4] = stone_type;
        map[1][5] = stone_type;
        map[4][5] = stone_type;
        map[10][15] = stone_type;

        map[10][16] = stone_type;
        map[10][17] = stone_type;
        map[11][15] = stone_type;
        map[11][17] = stone_type;
        map[12][15] = stone_type;
        map[12][16] = stone_type;
        map[12][17] = stone_type;

        map[17][22] = stone_type;
        map[17][21] = stone_type;
        map[17][20] = stone_type;
        map[17][19] = stone_type;
        map[17][18] = stone_type;

        map[18][17] = stone_type;
        map[7][8] = stone_type;
        map[7][6] = stone_type;
        map[15][22] = stone_type;
        map[5][28] = stone_type;
        map[8][25] = stone_type;
        map[19][14] = stone_type;
        map[19][14] = stone_type;
        map[19][15] = stone_type;
        map[19][18] = stone_type;
        map[15][15] = stone_type;
        map[14][5] = stone_type;
        map[14][6] = stone_type;
        map[15][4] = stone_type;
        map[15][7] = stone_type;
        map[16][5] = stone_type;
        map[16][6] = stone_type;
        map[15][29] = stone_type;
        map[16][29] = stone_type;
        map[18][29] = stone_type;
        map[19][29] = stone_type;
        map[6][9] = stone_type;
        map[10][19] = stone_type;
        map[10][20] = stone_type;
        map[11][19] = stone_type;
        map[11][20] = stone_type;
        map[8][23] = stone_type;
        map[16][6] = stone_type;
        map[17][16] = stone_type;

        map[4][13] = stone_type;
        map[3][14] = stone_type;
        map[4][15] = stone_type;
        map[3][16] = stone_type;
        map[4][17] = stone_type;
        map[3][18] = stone_type;

        total_diamonds = 0;
        for (int y = 0; y < rows; y++)
            for (int x = 0; x < cols; x++)
                if (map[y][x] == diamond_type)
                    total_diamonds++;

        diamonds_left = total_diamonds;

        //=========== physics (kiro )=============
        // البلاير في العمود رقم 1 وفي الصف رقم 1وده بيحددلي مكانه علي الجريد
        p.grid_x = 1;
        p.grid_y = 1;
        // ده مكان البلاير قعلا علي الشاشه بالكسلس
        p.visual_x = cell_size;
        p.visual_y = cell_size;
        // المكان اللي عايز يوصلو هو فيه اصلا فهو كدا واقف
        p.target_x = p.visual_x;
        p.target_y = p.visual_y;
        // دي سرعته في الثانيه
        p.move_speed = 300.f;
        p.moving = false;
    };
    //// kiro khaled
    Entity player_data;
    reset_game(player_data);

    Clock clock;
    float timeAccumulator = 0.f;

    //================ kiro ashraf  (if textures fail) =================
    RectangleShape player_rect(Vector2f(cell_size, cell_size));
    player_rect.setFillColor(Color::Green);

    //================ explo (donna)=================
    bool death_explosion_triggered = false;

    //================ MAIN LOOP =================
    bool restart_playing = false;
    while (Window.isOpen())
    {
        //===================================(Marcellino)===================================

        if (restart_playing) // bool true
        {
            if (HWA_KOL_SHWAYA_A3EED.getStatus() == sf::Music::Stopped)
            {
                if (gameplay_music_loaded)
                    gameplay_music.play();

                restart_playing = false;
            }
        }

        // لضمان ان المزيكا لو وقفت لاي سبب من الاسباب من نفسها و انت مش متحكم تشتغل لوحدها تاني(Marcellino )
        if ((currentstate == menu || currentstate == options) && yala_ya_yasser_sama3_3mo.getStatus() == Music::Stopped)
        {
            yala_ya_yasser_sama3_3mo.play();
        }
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f)
            dt = 0.1f;

        Event event;
        while (Window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                Window.close();

            // =================== press esc to quit to main menu while u are playing (marcellino) ===========
            if (currentstate == playing && event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
                    gameplay_music.stop(); // لو معملتش دي الاغنيتين هيدخلوا جوا بعض
                    currentstate = menu;
                }
            }

            //================ Main menu menu controls (marcellino) =================
            if (currentstate == menu && event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Down)
                {

                    // اذا ذا بليس ات ويتشز ذا ارووو ستاندز.. بصفر --> فالبلص واحد الي فالكوندشن دا عشان لما يدوس *داون* ينزل
                    // نيجي بقي لاهم حاجة الموديوليس اربعة دي محطوطة عشان لو السهم عند اخر اوبشن وداس داون تاني لا كدا يقوم السهم طالع علي اول اوبشن طب ليه؟
                    // فاول ما توصل ل اربعة و تعدي الحدود الموديولس اربعة يقولها لا و يرجعها للصفر
                    the_place_at_which_arrow_stands = (the_place_at_which_arrow_stands + 1) % 4;
                }
                if (event.key.code == Keyboard::Up)
                {
                    // نفس اللوجيك بس العكس
                    the_place_at_which_arrow_stands = (the_place_at_which_arrow_stands + 3) % 4;
                }

                if (event.key.code == Keyboard::Enter)
                {
                    if (the_place_at_which_arrow_stands == 0)
                    {

                        currentstate = instructions_state;
                        Level_arrow_index = 0;
                    }
                    else if (the_place_at_which_arrow_stands == 2)
                    {
                        currentstate = options;
                        place_of_elgwhara = 0;
                    }
                    else if (the_place_at_which_arrow_stands == 3)
                    {
                        Window.close();
                    }
                }
            }

            //================ option cont (Marcellino) =================
            else if (event.type == Event::KeyPressed && currentstate == options)
            {
                if (event.key.code == Keyboard::Down)
                {

                    //    نفس اللوجيك فات في مؤشر المين منيو الي فات لضمان عدم الخروج من الحدود
                    place_of_elgwhara = (place_of_elgwhara + 1) % 5;
                }
                else if (event.key.code == Keyboard::Up)
                {
                    place_of_elgwhara = (place_of_elgwhara + 4) % 5;
                }
                else if (event.key.code == Keyboard::Right && place_of_elgwhara == 1)
                {
                    if (cursorr.getPosition().x < maxX) // تأكد إن الكيرسور لسه ماوصلش لأقصى اليمين

                    {
                        cursorr.move(5.f, 0.f); // تحريك الكيرسور خمس فريمات علي الشاشة
                        float vol = audio_volume_UP_and_DOWN(cursorr.getPosition().x, minX, maxX);
                        yala_ya_yasser_sama3_3mo.setVolume(vol);
                    }
                }
                else if (event.key.code == Keyboard::Left && place_of_elgwhara == 1)
                {
                    if (cursorr.getPosition().x > minX) // بنتاكد ان الكيرسور موصلش اقصي الشمال
                    {
                        cursorr.move(-5.f, 0.f); // ارجع بالكيرسور خمس فريمات ورا
                        float vol = audio_volume_UP_and_DOWN(cursorr.getPosition().x, minX, maxX);
                        yala_ya_yasser_sama3_3mo.setVolume(vol);
                    }
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    switch (place_of_elgwhara)
                    {
                    case 0:
                    {
                        if (yala_ya_yasser_sama3_3mo.getStatus() == Music::Playing)
                        {
                            yala_ya_yasser_sama3_3mo.pause();
                        }
                        else if (yala_ya_yasser_sama3_3mo.getStatus() == Music::Paused)
                        {
                            yala_ya_yasser_sama3_3mo.play();
                        }
                        break;
                    }
                    case 4:
                    {
                        currentstate = menu;
                        place_of_elgwhara = 0;
                        break;
                    }
                    }
                }
            }
            // ========================= instructions & GAME_LEVEL (Marcellino) ==============================
            else if (currentstate == instructions_state && event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Enter)
                {
                    currentstate = game_level;
                }
            }
            else if ((currentstate == game_level && event.type == Event::KeyPressed))
            {
                if (event.key.code == Keyboard::Down)
                {
                    Level_arrow_index = (Level_arrow_index + 1) % 3;
                }
                if (event.key.code == Keyboard::Up)
                {
                    Level_arrow_index = (Level_arrow_index + 2) % 3;
                }

                if (event.key.code == Keyboard::Enter && Level_arrow_index == 0)
                {
                    currentstate = playing;
                    yala_ya_yasser_sama3_3mo.stop();
                    YALA_BENA.play();
                    reset_game(player_data);
                    gamedata["Collected"] = 22;
                    gamedata["Points"] = 0;
                    gamedata["Time"] = 200;
                    // sound ell3baa
                    if (gameplay_music_loaded)
                        gameplay_music.play();
                    game_state = state_playing;
                    death_explosion_triggered = false;
                    clock.restart();
                }
                if (event.key.code == Keyboard::Enter && Level_arrow_index == 1)
                {
                    currentstate = playing;
                    yala_ya_yasser_sama3_3mo.stop();
                    YALA_BENA.play();
                    reset_game(player_data);
                    gamedata["Collected"] = 22;
                    gamedata["Points"] = 0;
                    gamedata["Time"] = 70;
                    // sound ell3ba
                    if (gameplay_music_loaded)
                        gameplay_music.play();

                    game_state = state_playing;
                    death_explosion_triggered = false;
                    clock.restart();
                }
                if (event.key.code == Keyboard::Enter && Level_arrow_index == 2)
                {
                    currentstate = playing;
                    yala_ya_yasser_sama3_3mo.stop();
                    YALA_BENA.play();
                    reset_game(player_data);
                    gamedata["Collected"] = 22;
                    gamedata["Points"] = 0;
                    gamedata["Time"] = 35;

                    // sound ell3ba
                    if (gameplay_music_loaded)
                        gameplay_music.play();

                    game_state = state_playing;
                    death_explosion_triggered = false;
                    clock.restart();
                }
            }

            //================ ( Marcellino // Kirollos khalled // Mohra Magdy // Mohra Bolous) =================
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::R && currentstate == playing)
            {
                ya_Fashel.stop();
                YALLAHWAY.stop();
                gameplay_music.stop();

                reset_game(player_data);

                game_state = state_playing;
                death_explosion_triggered = false;

                // تشغيل صوت الريستارت
                HWA_KOL_SHWAYA_A3EED.stop();
                HWA_KOL_SHWAYA_A3EED.play();
                restart_playing = true;

                yalahway_played = false;
                died_by_time = false;

                if (Level_arrow_index == 0)
                {
                    gamedata["Collected"] = 22;
                    gamedata["Points"] = 0;
                    gamedata["Time"] = 200;
                }
                if (Level_arrow_index == 1)
                {
                    gamedata["Collected"] = 22;
                    gamedata["Points"] = 0;
                    gamedata["Time"] = 70;
                }
                if (Level_arrow_index == 2)
                {
                    gamedata["Collected"] = 22;
                    gamedata["Points"] = 0;
                    gamedata["Time"] = 35;
                }
            }

            //================ (MARCELLINO sound effects) =================
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape && currentstate == playing)
            {
                currentstate = menu;
                if (yala_ya_yasser_sama3_3mo.getStatus() != Music::Playing)
                {
                    yala_ya_yasser_sama3_3mo.play();
                }
            }
        }

        //================ game logic ( kirollos khaled // Marcellino (Marcellino shared onlyy to link main menu with game) ) =================
        if (currentstate == playing)
        {
            yalahway_played = false;
            died_by_time = false;
            // لمنع تشغيل نغمة يلهوييي يلهويييي اثناء اللعب
            // Update explosions
            explosionManager.update(dt);

            // Time countdown (Map of gamedata ----> Marcellino)

            timeAccumulator += dt;
            while (timeAccumulator >= 1.f)
            {
                decreasing_time(gamedata);
                timeAccumulator -= 1.f;
                //  الكود دا كاتبه عشان الوقت الي في اللعبة يبقي ثابت ويمشي حسب الزمن الحقيقي
                //   من غيره : الوقت هيعتمد على سرعة الجهاز وهيبقى أسرع أو أبطأ حسب الـفريمات : اف بي اس
            }

            // Check time out
            if (gamedata["Time"] <= 0 && game_state == state_playing)
            {

                game_state = state_dead;
                died_by_time = true;
                death_explosion_triggered = false;

                gameplay_music.stop();

                if (!yalahway_played)
                {
                    YALLAHWAY.stop(); //  احتياطي عشان المزيكا تبدا من الاول
                    YALLAHWAY.play();
                    yalahway_played = true;
                }
            }

            int dx = 0, dy = 0;
            /// movement (Madonna)
            if (game_state == state_playing)
            {
                ya_Fashel.stop();
                if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
                    dx = 1;
                else if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
                    dx = -1;
                else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
                    dy = -1;
                else if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
                    dy = 1;

                //================ face direction (Kirollos khaled) =================
                if (dx != 0)
                    facing_right = (dx > 0);

                //================ movment (kirollos khaled ) =================

                // اللاعب هنا مش بيتحرك في بسinput حركه جديده
                if (!player_data.moving && (dx != 0 || dy != 0))
                {
                    //  ده المكان الجديد اللي عايز علي الجريد
                    int nx = player_data.grid_x + dx;
                    int ny = player_data.grid_y + dy;
                    // نشوف لو البلاير اتحرك الحركه دي هيخبط في حيطه او صخره ولا
                    if (nx >= 0 && nx < cols && ny >= 0 && ny < rows)
                    {
                        if (map[ny][nx] != wall_type && map[ny][nx] != stone_type)
                        {
                            //  لو فيتحرك عادي وكدا هو وصل لمكانه الجديد علي الجريد
                            player_data.grid_x = nx;
                            player_data.grid_y = ny;
                            // ده المكان اللي هيوصله يبقا باين علي الشاشه بالبكسلس
                            player_data.target_x = nx * cell_size;
                            player_data.target_y = ny * cell_size;

                            player_data.moving = true;
                        }
                    }
                }

                //================ smooth and conditions  (kirollos Khaled // kirollos Ashraf ) =================
                float step = player_data.move_speed * dt;

                if (player_data.visual_x < player_data.target_x)
                    player_data.visual_x = std::min(player_data.visual_x + step, player_data.target_x);

                if (player_data.visual_x > player_data.target_x)
                    player_data.visual_x = std::max(player_data.visual_x - step, player_data.target_x);

                if (player_data.visual_y < player_data.target_y)
                    player_data.visual_y = std::min(player_data.visual_y + step, player_data.target_y);

                if (player_data.visual_y > player_data.target_y)
                    player_data.visual_y = std::max(player_data.visual_y - step, player_data.target_y);

                if (player_data.visual_x == player_data.target_x &&
                    player_data.visual_y == player_data.target_y)
                {
                    if (map[player_data.grid_y][player_data.grid_x] == sand_type)
                    {
                        map[player_data.grid_y][player_data.grid_x] = empty_type;
                    }

                    if (map[player_data.grid_y][player_data.grid_x] == diamond_type)
                    {
                        // EXPLOSION EFFECT: Diamond collected! (donna and kiro khaled)
                        float explosion_x = player_data.grid_x * cell_size + cell_size / 2;
                        float explosion_y = player_data.grid_y * cell_size + cell_size / 2;
                        explosionManager.addDiamondExplosion(explosion_x, explosion_y);

                        diamonds_left--;
                        increasing_collected(gamedata);
                        increasing_points(gamedata);

                        map[player_data.grid_y][player_data.grid_x] = empty_type;
                    }
                    player_data.moving = false;
                    if (diamonds_left == 0 &&
                        player_data.grid_x == 1 &&
                        player_data.grid_y == 1)
                    {
                        game_state = state_win;
                        gameplay_music.stop();
                        ZAGHRATY_YA_7AGA.play();
                    }
                }

                if (texturesLoaded)
                {
                    player_spr.setPosition(player_data.visual_x, player_data.visual_y);

                    //================ FLIP(kiro khaled) =================
                    if (facing_right)
                    {
                        player_spr.setScale(base_scale_x, base_scale_y);
                        player_spr.setOrigin(0, 0);
                    }
                    else
                    {
                        player_spr.setScale(-base_scale_x, base_scale_y);
                        player_spr.setOrigin(player_tex.getSize().x, 0);
                    }
                }
                else
                {
                    player_rect.setPosition(player_data.visual_x, player_data.visual_y);
                }

                //================ GRAVITY (kiro and kiro-->(conditions )   ) =================
                // دي عملاها كدا عشان يبان الشكل الجديدبعد الحركه
                int next_map[rows][cols];
                // عشان نحدث الحركه الجديده في الماب
                for (int y = 0; y < rows; y++)
                    for (int x = 0; x < cols; x++)
                        next_map[y][x] = map[y][x];

                float fall_speed = 300.f;

                for (int y = rows - 2; y >= 1; y--)
                {
                    for (int x = 1; x < cols - 1; x++)
                    {
                        int type = map[y][x];
                        if (type != stone_type && type != diamond_type)
                            continue;

                        if (map[y + 1][x] == empty_type)
                        {
                            fall_offset[y][x] += fall_speed * dt;

                            //================ NEW: COLLECT FALLING DIAMONDS WHEN MOVING UP =================
                            // Check if player is moving up into a falling diamond (kiro and kiro  )
                            if (type == diamond_type &&
                                player_data.grid_x == x &&
                                player_data.grid_y == y &&
                                dy == -1 &&
                                fall_offset[y][x] > 0)
                            {
                                // Player is moving up into a falling diamond - collect it!
                                float explosion_x = x * cell_size + cell_size / 2;
                                float explosion_y = y * cell_size + cell_size / 2;
                                explosionManager.addDiamondExplosion(explosion_x, explosion_y);

                                diamonds_left--;
                                increasing_collected(gamedata);
                                increasing_points(gamedata);

                                if (diamonds_left == 0)
                                    game_state = state_win;

                                next_map[y][x] = empty_type;
                                fall_offset[y][x] = 0;
                                continue;
                            }

                            if (player_data.grid_x == x && player_data.grid_y == y + 1)
                            {
                                if (fall_offset[y][x] > cell_size * 0.9f && !player_data.moving)
                                {
                                    game_state = state_dead;
                                    death_explosion_triggered = false;
                                }
                            }

                            if (fall_offset[y][x] >= cell_size)
                            {
                                next_map[y][x] = empty_type;
                                next_map[y + 1][x] = type;

                                fall_offset[y][x] = 0;
                                fall_offset[y + 1][x] = 0;
                            }
                        }
                        else
                        {
                            fall_offset[y][x] = 0;
                        }
                    }
                }

                for (int y = 0; y < rows; y++)
                    for (int x = 0; x < cols; x++)
                        map[y][x] = next_map[y][x];
            }

            // Trigger death explosion once( kiro khaled & Donna  )
            if (game_state == state_dead && !death_explosion_triggered)
            {
                float explosion_x = player_data.visual_x + cell_size / 2;
                float explosion_y = player_data.visual_y + cell_size / 2;
                explosionManager.addDeathExplosion(explosion_x, explosion_y);
                //============= (Marcellino) addition ===================
                gameplay_music.stop();

                if (!died_by_time) // لو خسر الجيم ان زلطة او جوهرة وقعت عليه شغل يا فاشل لاكن لو خسر بالوقت هيشغل يلهوييييي
                    ya_Fashel.play();
                //=======================================================

                death_explosion_triggered = true;
            }
        }

        //================ DRAW( Mohra Magdy  ) =================
        Window.clear(Color::Black);

        if (currentstate == menu)
        {
            Window.draw(menuBg);
            arrowPOINTER.setPosition(MARCELLINO[the_place_at_which_arrow_stands]);
            Window.draw(arrowPOINTER);

            devText.move(-devSpeed * dt, 0.f);
            if (devText.getPosition().x + devWidth < 0)
            {
                devText.setPosition(window_width, window_height - 40);
            }
            Window.draw(devText);
            //===========================================================
            // ======================(Marcellino)========================
            // TO GUARANTEE THE NON - CONFUSION OF MUSIC
            // TO GUARANTEE THE NON - CONFUSION OF MUSIC
            // TO GUARANTEE THE NON - CONFUSION OF MUSIC
            if (HWA_KOL_SHWAYA_A3EED.getStatus() == Music::Playing)
            {
                HWA_KOL_SHWAYA_A3EED.stop();
            }
            if (gameplay_music.getStatus() == Music::Playing)
            {
                gameplay_music.stop();
            }
            if (YALLAHWAY.getStatus() == Music::Playing)
            {
                YALLAHWAY.stop();
            }
            if (ya_Fashel.getStatus() == Music::Playing)
            {
                ya_Fashel.stop();
            }
            // TO GUARANTEE THE NON - CONFUSION OF MUSIC
            // TO GUARANTEE THE NON - CONFUSION OF MUSIC
            // TO GUARANTEE THE NON - CONFUSION OF MUSIC
            //============================================================
        }

        // لضمان ان شريط الاسماء في الاوبشن يبان (Mohra Bolous)
        else if (currentstate == options)
        {
            display_options(Window, OPBg, arrowPOINTER, place_of_elgwhara, ON_photo,
                            OFF_Photo, yala_ya_yasser_sama3_3mo, volumee, cursorr, no_volumee);

            devText.move(-devSpeed * dt, 0.f);
            if (devText.getPosition().x + devWidth < 0)
            {
                devText.setPosition(window_width, window_height - 40);
            }
            Window.draw(devText);
        }
        else if (currentstate == instructions_state)
        {
            Window.draw(INBG);
        }

        else if (currentstate == game_level)
        {
            Window.draw(game_levelbg);

            arrowPOINTER.setPosition(
                LEVEL_POSITIONS[Level_arrow_index][0],
                LEVEL_POSITIONS[Level_arrow_index][1]);
            // same idea of the previous
            Window.draw(arrowPOINTER);
        }
        else if (currentstate == playing)
        {
            yalahway_played = false;
            died_by_time = false;
            // لمنع تشغيل نغمة يلهويي يلهوييي اثناء اللعب
            if (diamonds_left == 0 && game_state != state_win)
            {

                if (texturesLoaded)
                {
                    door_spr.setPosition(1 * cell_size, 1 * cell_size);
                    Window.draw(door_spr);
                }
            }
            // Draw tile
            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {
                    int t = map[y][x];
                    if (texturesLoaded)
                    {
                        if (t == wall_type)
                        {
                            wall_spr.setPosition(x * cell_size, y * cell_size + fall_offset[y][x]);
                            Window.draw(wall_spr);
                        }
                        else if (t == sand_type)
                        {
                            sand_spr.setPosition(x * cell_size, y * cell_size + fall_offset[y][x]);
                            Window.draw(sand_spr);
                        }
                        else if (t == diamond_type)
                        {
                            diamond_spr.setPosition(x * cell_size, y * cell_size + fall_offset[y][x]);
                            Window.draw(diamond_spr);
                        }
                        else if (t == stone_type)
                        {
                            stone_spr.setPosition(x * cell_size, y * cell_size + fall_offset[y][x]);
                            Window.draw(stone_spr);
                        }
                    }
                    else
                    {
                        // Fallback colored rectangles (kiro ashraf)
                        if (t != empty_type)
                        {
                            RectangleShape tile(Vector2f(cell_size, cell_size));
                            if (t == wall_type)
                                tile.setFillColor(Color(120, 120, 120));
                            else if (t == sand_type)
                                tile.setFillColor(Color(150, 100, 40));
                            else if (t == diamond_type)
                                tile.setFillColor(Color(0, 180, 255));
                            else if (t == stone_type)
                                tile.setFillColor(Color(160, 160, 160));

                            tile.setPosition(x * cell_size, y * cell_size + fall_offset[y][x]);
                            Window.draw(tile);
                        }
                    }
                }
            }

            // Draw player (only if alive)(kirollos Khaled // kirollos Ashraf )
            if (game_state != state_dead)
            {
                if (texturesLoaded)
                    Window.draw(player_spr);
                else
                    Window.draw(player_rect);
            }

            // DRAW EXPLOSIONS (rendered on top of everything for best visibility)(  kirollos khaled )
            explosionManager.draw(Window);

            // Draw UI( & kirollos khaled)
            Window.draw(diamonds_text);

            // Draw UI Bar(Marcellino)
            Window.draw(uiBar);
            stringstream ss;
            // constants + variables in a string 
            ss << "TIME " << setfill('0') << setw(3) << gamedata["Time"] << "                            "
               << "GEMS " << setfill('0') << setw(3) << gamedata["Collected"] << "                      "
               << "POINTS " << setfill('0') << setw(7) << gamedata["Points"];
            uiText.setString(ss.str());
            Window.draw(uiText);

            // Game over/win screens(kiro Ashraf)
            if (game_state == state_dead)
                Window.draw(game_over_text);
            if (game_state == state_win)
                Window.draw(win_text);
        }

        Window.display();
    }

    return 0;
}

//____________________======================************ The End *************=================______________________

// Members Participated in this Code :
//######################################
// 1- Kirollos Khaled
// 2- Kirollos Ashraf
// 3- Daniel Mina
// 4- Madonna Magdy
// 5- Mohra Magdy
// 6- Mohra Bolous
// 7- Marcellino Wadie