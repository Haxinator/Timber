//libraries
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>


//makes code easier to type
using namespace sf;

//Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
const int NUM_TREES = 4;
const int NUM_CLOUDS = 3;

int frames = 0;
float frameRate = 0;

Sprite branches[NUM_BRANCHES];

//where is the player/branch?
//let or Right
enum class side {LEFT, RIGHT, NONE};

side branchPositions[NUM_BRANCHES];

// This is where our game starts from.
int main()
{
    //create video mode object
    //VideoMode vm(1920,1080);
    // Create and open a window for the game
    // RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

    // Low res code
    VideoMode vm(960, 540);

    // Low res code
    RenderWindow window(vm, "Timber!!!", Style::Fullscreen);
    View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);
    
    //create a texture to hold a graphic on GPU
    Texture textureBackground;
    
    //load graphic into the texture
    textureBackground.loadFromFile("graphics/background.png");

    //create a sprite
    Sprite spriteBackground;

    //Attach the texture to the sprite
    spriteBackground.setTexture(textureBackground);

    //Set the spriteBackground to cover the screen
    spriteBackground.setPosition(0, 0);

    //Make a tree sprite
    Texture textureTree;
    textureTree.loadFromFile("graphics/tree.png");
    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);

    Sprite backgroundTrees[4];

    backgroundTrees[0].scale(Vector2f(0.7, 1));
    backgroundTrees[1].scale(Vector2f(0.9, 1));
    backgroundTrees[2].scale(Vector2f(0.5, 1));
    backgroundTrees[3].scale(Vector2f(0.8, 1));


    backgroundTrees[0].setPosition(500, -100);
    backgroundTrees[1].setPosition(1500, -150);
    backgroundTrees[2].setPosition(250, -200);
    backgroundTrees[3].setPosition(5, -20);

    for (int i = 0; i < NUM_TREES; i++)
    {
        backgroundTrees[i].setTexture(textureTree);
    }



    //Make a bee sprite
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee.png");
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(-100, 800);

    //Is the bee currently moving?
    bool beeActive = false;

    //How fast can the bee fly?
    float beeSpeed = 0.0f;
    float beeSpeedY = 0.0f;

    //make 3 clouds from one texture
    Texture textureCloud;

    //load texture
    textureCloud.loadFromFile("graphics/cloud.png");

    Sprite spriteClouds[3];

    //make cloud sprites with the same texture

    spriteClouds[0].setTexture(textureCloud);
    spriteClouds[1].setTexture(textureCloud);
    spriteClouds[2].setTexture(textureCloud);

    //Position the clouds on the left of the screen
    //at different heights.
    spriteClouds[0].setPosition(-100, 0);
    spriteClouds[1].setPosition(-100, 250);
    spriteClouds[2].setPosition(-100, 500);

    //Are the clouds currently on screen?
    bool cloudActive[3] = { false, false, false };

    //How fast is each cloud?
    float cloudSpeeds[3] = { 0.0f, 0.0f, 0.0f };

    //variables to control time
    Clock clock;

    //time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    //Check whether game is paused
    bool paused = true;

    //draw text
    //starting score
    int score = 0;

    //load font
    Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    //text for SFML to display string
    Text messageText;
    Text scoreText;
    Text frameText;

    //assign string
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");
    frameText.setString("FPS = 0");

    //assign size
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);
    frameText.setCharacterSize(100);

    //set colour
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);
    frameText.setFillColor(Color::White);

    //set the font
    messageText.setFont(font);
    scoreText.setFont(font);
    frameText.setFont(font);

    //Position the text
    FloatRect textRect = messageText.getLocalBounds();

    messageText.setOrigin(textRect.left +
        textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);

    messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

    scoreText.setPosition(20, 20);
    frameText.setPosition(1200, 20);

    //background colour for text
    RectangleShape scoreShader;
    RectangleShape frameShader;

    //shader settings.
    //black and semi-transparent.
    scoreShader.setFillColor(Color(0,0,0, 128)); 
    scoreShader.setSize(Vector2f(scoreText.getLocalBounds().width * 1.3, scoreText.getLocalBounds().height * 1.3));
    scoreShader.setPosition(20, 30);
    
    frameShader.setFillColor(Color(0, 0, 0, 128));
    frameShader.setSize(Vector2f(frameText.getLocalBounds().width * 2, frameText.getLocalBounds().height * 1.3));
    frameShader.setPosition(1200, 30);

    //Prepare 6 branches
    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");

    //Set the texture for each branch Sprite
    for (int i = 0; i < NUM_BRANCHES; i++)
    {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);

        //Set the sprite's origin to dead centre
        //We can then spin it round without changing its position.
        branches[i].setOrigin(220, 20);
    }


    //Prepare the player
    Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580, 720);

    //The player starts on the left
    side playerSide = side::LEFT;

    //prepare gravestone
    Texture textureRIP;
    textureRIP.loadFromFile("graphics/rip.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(2000, 860);

    //Prepare axe
    Texture textureAxe;
    textureAxe.loadFromFile("graphics/axe.png");
    Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(2000, 830);

    //Line axe up with tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    //prepare the flying log
    Texture textureLog;
    textureLog.loadFromFile("graphics/log.png");
    Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810, 720);

    //log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    //Control player input
    bool acceptInput = false;

    //Prepare the sounds
    //The player chopping sounds
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("sound/chop.wav");
    Sound chop;
    chop.setBuffer(chopBuffer);

    //The player met his end under a branch
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/death.wav");
    Sound death;
    death.setBuffer(deathBuffer);

    //Out of time
    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("sound/out_of_time.wav");
    Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);

    while (window.isOpen())
    {
/*
        ********************************
        Handle players input
        ********************************
        */

        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::KeyReleased && !paused)
            {
                //Listen for key presses again
                acceptInput = true;

                //hide the axe
                spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        //unpause
        if (Keyboard::isKeyPressed(Keyboard::Return))
        {
            paused = false;

            //reset time and score
            score = 0;
            timeRemaining = 6;

            //Make sure all the branches disappear
            //starting in the second position
            for (int i = 1; i < NUM_BRANCHES; i++)
            {
                branchPositions[i] = side::NONE;
            }

            //Make sure the gravestone is hidden
            spriteRIP.setPosition(675, 2000);

            //Move the player into position
            spritePlayer.setPosition(580, 720);
            spritePlayer.setColor(Color(255, 255, 255, 255));

            acceptInput = true;
        }

        //Wrap the player controls to
        //make sure we are accepting input
        if (acceptInput)
        {
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                //make syre the player is on the right
                playerSide = side::RIGHT;
                score++;

                //add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(AXE_POSITION_RIGHT,
                    spriteAxe.getPosition().y);

                spritePlayer.setPosition(1200, 720);

                //update branches
                updateBranches(score);

                //Set the log flying to the left
                spriteLog.setPosition(810, 720);
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                //play chop sound
                chop.play();
            }

            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                //make sure the player is on the left
                playerSide = side::LEFT;

                score++;

                //Add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(AXE_POSITION_LEFT,
                    spriteAxe.getPosition().y);

                spritePlayer.setPosition(580, 720);

                //update branches
                updateBranches(score);

                //set log flying
                spriteLog.setPosition(810, 720);
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

                chop.play();
            }
        }

        /*
        *********************************
        update scene
        *********************************
        */
        if (!paused)
        {
            //measure time taken between each frame
            Time dt = clock.restart();

            //Subtract from the amount of time remaining
            timeRemaining -= dt.asSeconds();
            //size up the time bar
            timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining,
                timeBarHeight));

            if (timeRemaining <= 0.0f) {
                //Pause the game
                paused = true;

                //Change the message shown to player
                messageText.setString("OUT OF TIME!!");

                //Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);

                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                //play out of time sound

                outOfTime.play();
            }

            //Setup the bee
            if (!beeActive)
            {
                //How fast is the bee
                srand((int)time(0));
                beeSpeed = (rand() % 200) + 200;
                beeSpeedY = beeSpeed / 2;

                //how high is the bee
                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);
                beeActive = true;
            }
            else {

                //make bee Y direction alternate to give it more "life"
                int beeDirection = (rand() % 2);

                if (beeDirection > 0)
                {
                    beeSpeedY *= -1;
                }

                //move the bee
                spriteBee.setPosition(
                    spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
                    spriteBee.getPosition().y - (beeSpeedY * dt.asSeconds()));

                //has the bee reached the left-hand edge of the screen?
                if (spriteBee.getPosition().x < -100)
                {
                    //Set it up ready to be a whole new bee next frame;
                    beeActive = false;
                }
            }

            //Manage the clouds
            for (int i = 0; i < NUM_CLOUDS; i++) //3 clouds
            {
                if (!cloudActive[i])
                {
                    //how fast is the cloud
                    srand((int)time(0)* (10 * (1 + i))); //ensure different seed for each cloud
                    cloudSpeeds[i] = (rand() % 200);

                    //how high is cloud
                    srand((int)time(0)* (10 * (1 + i)));
                    float height = (rand() % 150);
                    spriteClouds[i].setPosition(-300, height);
                    cloudActive[i] = true;
                }
                else {
                    spriteClouds[i].setPosition(spriteClouds[i].getPosition().x +
                        (cloudSpeeds[i] * dt.asSeconds()),
                        spriteClouds[i].getPosition().y);

                    //has the cloud reached the right hand edge of the screen?
                    if (spriteClouds[i].getPosition().x > 1920)
                    {
                        //Set it up to be a new cloud next frame
                        cloudActive[i] = false;
                    }
                }
 
            }

            //update text every 100 frames.
            if (frames % 100 == 0)
            {
                //Update the score text
                std::stringstream ss;
                ss << "score = " << score;
                scoreText.setString(ss.str());

                frameRate = 1.0/dt.asSeconds();

                std::stringstream framess;
                framess << "FPS = " << frameRate;
                frameText.setString(framess.str());
            }

            //update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++)
            {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT)
                {
                    //Move the sprite to the left side
                    branches[i].setPosition(610, height);

                    //Flip the sprite round the other way
                    branches[i].setRotation(180);
                }
                else if (branchPositions[i] == side::RIGHT) 
                {
                    //Move the sprite to the right side
                    branches[i].setPosition(1330, height);

                    //Set the sprite rotation to normal
                    branches[i].setRotation(0);
                }
                else 
                {
                    //Hide branch
                    branches[i].setPosition(3000, height);
                }
            }

            //Handle flying log
            if (logActive)
            {
                spriteLog.setPosition(
                    spriteLog.getPosition().x +
                    (logSpeedX * dt.asSeconds()),
                    spriteLog.getPosition().y +
                    (logSpeedY * dt.asSeconds()));

                //Has the log reached the right hand edge?
                if(spriteLog.getPosition().x < -100 || 
                    spriteLog.getPosition().x > 2000)
                {
                    //Set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition(810, 720);
                }

            }

            //has the player been squished by a branch?
            if (branchPositions[5] == playerSide)
            {
                //death
                paused = true;
                acceptInput = false;

                //draw the gravestone
                spriteRIP.setPosition(525, 760);

                //hide axe
                spriteAxe.setPosition(2000, 830);

                //hide flying log
                spriteLog.setPosition(2000, 0);

                //hide the player
                //spritePlayer.setPosition(2000, 660);
                spritePlayer.setColor(Color(255, 255, 255, 125));

                //change text of message
                messageText.setString("SQUISHED!!");

                //center it on the screen
                FloatRect textRect = messageText.getLocalBounds();

                messageText.setOrigin(textRect.left +
                    textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);

                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                //play death sound
                death.play();
            }
        }

        /*
        *********************************
        draw scene
        *********************************
        */

        //clear everything from the last frame
        window.clear();

        //draw our game scene here.
        window.draw(spriteBackground);

        //draw clouds
        for (int i = 0; i < NUM_CLOUDS; i++)
        {
            window.draw(spriteClouds[i]);
        }

        for (int i = 0; i < NUM_TREES; i++)
        {
            window.draw(backgroundTrees[i]);
        }

        //Draw the branches
        for (int i = 0; i < NUM_BRANCHES; i++)
        {
            window.draw(branches[i]);
        }

        //Draw the flying log
        window.draw(spriteLog);

        //draw tree
        window.draw(spriteTree);

        //draw the graveStone
        window.draw(spriteRIP);

        //Draw the player
        window.draw(spritePlayer);

        //Draw the axe
        window.draw(spriteAxe);

        //draw bee
        window.draw(spriteBee);


        window.draw(scoreShader);

        //draw score text
        window.draw(scoreText);

        window.draw(frameShader);

        window.draw(frameText);



        //draw timebar
        window.draw(timeBar);

        if (paused)
        {
            //draw message text
            window.draw(messageText);
        }

        frames++;

        //Show everything we just drew.
        window.display();
    }

    return 0;
}

void updateBranches(int seed)
{
    //Move all branches down one place
    for (int i = NUM_BRANCHES - 1; i > 0; i--)
    {
        branchPositions[i] = branchPositions[i - 1];
    }

    //Spawn a new branch at position 0
    //LEFT, RIGHT, or NONE
    srand((int)time(0) + seed);
    int r = (rand() % 5);

    switch (r) {
    case 0:
        branchPositions[0] = side::LEFT;
        break;
    case 1:
        branchPositions[0] = side::RIGHT;
        break;
    default:
        branchPositions[0] = side::NONE;
        break;
    }
}