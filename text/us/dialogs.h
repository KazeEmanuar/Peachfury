// Parameters: dialog enum ID, (unused), lines per box, left offset, width
//dialogs.h
#ifdef VERSION_EU
#define COMRADES "friends"
#define PLASTERED "splattered"
#define SCAM_ME "cheat!\n"
#define SCRAM "get lost"
#define YOU_CANT_SWIM_IN_IT "Its too heavy to swim\nwith."
#define GIVE_UP "give up"
#else
#define COMRADES "comrades"
#define PLASTERED "plastered"
#define SCAM_ME "scam\nME. "
#define SCRAM "scram--"
#define YOU_CANT_SWIM_IN_IT "You can't swim in it."
#define GIVE_UP "give"
#endif

DEFINE_DIALOG(DIALOG_000, 1, 5, 30, 200, _("\
This game was created\n\
to showcase my new\n\
RTYI optimizations.\n\
I've edited the entire\n\
SM64 source code for this.\n\
The SM64 decomp was\n\
a huge help in this,\n\
as it allows me to\n\
effeciently edit\n\
all of Mario 64's code.\n\
I applied the newest\n\
N64 knowledge so that\n\
levels are rendered up\n\
to 5 times faster than\n\
in the original game!\n\
It features a new\n\
camera, polished\n\
physics and can\n\
run up to 60 FPS,\n\
even on real N64!"))

DEFINE_DIALOG(DIALOG_001, 1, 4, 95, 200, _("\
Watch out! If you wander\n\
around here, you're liable\n\
to be " PLASTERED " by a\n\
water bomb!\n\
Those enemy Bob-ombs love\n\
to fight, and they're\n\
always finding ways to\n\
attack.\n\
This meadow has become\n\
a battlefield ever since\n\
the Big Bob-omb got his\n\
paws on the Power Star.\n\
Can you recover the Star\n\
for us? Cross the bridge\n\
and go left up the path\n\
to find the Big Bob-omb.\n\
Please come back to see\n\
me after you've retrieved\n\
the Power Star!"))

DEFINE_DIALOG(DIALOG_002, 1, 4, 95, 200, _("\
Hey, you! It's dangerous\n\
ahead, so listen up! Take\n\
my advice.\n\
\n\
Cross the two\n\
bridges ahead, then\n\
watch for falling\n\
water bombs.\n\
The Big Bob-omb at the\n\
top of the mountain is\n\
very powerful--don't let\n\
him grab you!\n\
We're Bob-omb Buddies,\n\
and we're on your side.\n\
You can talk to us\n\
whenever you'd like to!"))

DEFINE_DIALOG(DIALOG_003, 1, 5, 95, 200, _("\
Thank you, Mario! The Big\n\
Bob-omb is nothing but a\n\
big dud now! But the\n\
battle for the castle has\n\
just begun.\n\
Other enemies are holding\n\
the other Power Stars. If\n\
you recover more Stars,\n\
you can open new doors\n\
that lead to new worlds!\n\
My Bob-omb Buddies are\n\
waiting for you. Be sure\n\
to talk to them--they'll\n\
set up cannons for you."))

DEFINE_DIALOG(DIALOG_004, 1, 3, 95, 200, _("\
We're peace-loving\n\
Bob-ombs, so we don't use\n\
cannons.\n\
But if you'd like\n\
to blast off, we don't\n\
mind. Help yourself.\n\
We'll prepare all of the\n\
cannons in this course for\n\
you to use. Bon Voyage!"))

DEFINE_DIALOG(DIALOG_005, 1, 3, 30, 200, _("\
Hey, Mario! Is it true\n\
that you beat the Big\n\
Bob-omb? Cool!\n\
You must be strong. And\n\
pretty fast. So, how fast\n\
are you, anyway?\n\
Fast enough to beat me...\n\
Koopa the Quick? I don't\n\
think so. Just try me.\n\
How about a race to the\n\
mountaintop, where the\n\
Big Bob-omb was?\n\
Whaddya say? When I say\n\
『Go,』 let the race begin!\n\
\n\
Ready....\n\
\n\
//Go!////Don't Go"))

//parakarry
DEFINE_DIALOG(DIALOG_006, 1, 5, 30, 200, _("\
Mario, you lard!\n\
You really got\n\
wasted and used up\n\
all my money to order\n\
pizza and pasta!\n\
You're in debt,\n\
little man!\n\
You owe me...\n\
at least 25 stars!\n\
Buzz off and work!"))

DEFINE_DIALOG(DIALOG_007, 1, 5, 30, 200, _("\
Hufff...fff...pufff...\n\
Whoa! You...really...are...\n\
fast! A human blur!\n\
Here you go--you've won\n\
it, fair and square!"))

//sign
DEFINE_DIALOG(DIALOG_008, 1, 5, 30, 200, _("\
At least you paid\n\
your debt!\n\
But you're still not\n\
off the hook!\n\
\n\
Before I forgive you,\n\
you need to go and\n\
subscribe to\n\
Kaze Emanuar!!\n\
Teehee :)"))

DEFINE_DIALOG(DIALOG_009, 1, 5, 30, 200, _("\
Long time, no see! Wow,\n\
have you gotten fast!\n\
Have you been training\n\
on the sly, or is it the\n\
power of the Stars?\n\
I've been feeling down\n\
about losing the last\n\
race. This is my home\n\
course--how about a\n\
rematch?\n\
The goal is in\n\
Windswept Valley.\n\
Ready?\n\
\n\
//Go//// Don't Go"))

DEFINE_DIALOG(DIALOG_010, 1, 4, 30, 200, _("\
You've stepped on the\n\
Wing Cap Switch. Wearing\n\
the Wing Cap, you can\n\
soar through the sky.\n\
Now Wing Caps will pop\n\
out of all the red blocks\n\
you find.\n\
\n\
Would you like to Save?\n\
\n\
//Yes////No"))

DEFINE_DIALOG(DIALOG_011, 1, 5, 30, 200, _("\
You've just stepped on\n\
the Cloud Cap Switch!\n\
The Cloud Cap allows\n\
Mario to spawn clouds\n\
by pressing A in the air.\n\
Would you like to Save?\n\
\n\
//Yes////No"))

DEFINE_DIALOG(DIALOG_012, 1, 4, 30, 200, _("\
You've just stepped on\n\
the Ice Cap Switch.\n\
The Ice cap lets\n\
Mario freeze water.\n\
Would you like to Save?\n\
\n\
//Yes////No"))

DEFINE_DIALOG(DIALOG_013, 1, 5, 30, 200, _("\
You've collected 100\n\
coins! Mario gains more\n\
power from the island.\n\
Do you want to Save?\n\
//Yes////No"))

DEFINE_DIALOG(DIALOG_014, 1, 4, 30, 200, _("\
Wow! Another Power Star!\n\
Mario gains more courage\n\
from the power of the\n\
castle.\n\
Do you want to Save?\n\
\n\
//You Bet//Not Now"))

//mouse captain act1
DEFINE_DIALOG(DIALOG_015, 1, 3, 30, 200, _("\
Howdy champ!\n\
My team is mining\n\
in here.\n\
Watch out in those mines\n\
we've already lost a\n\
few mice to its perils!"))

//mouse diamon1 act1
DEFINE_DIALOG(DIALOG_016, 1, 4, 30, 200, _("\
Don't sneak up!\n\
It's scary enough as is\n\
with some of us going\n\
missing!"))

DEFINE_DIALOG(DIALOG_017, 1, 4, 30, 200, _("\
I'm the Big Bob-omb, lord\n\
of all blasting matter,\n\
king of ka-booms the\n\
world over!\n\
How dare you scale my\n\
mountain? By what right\n\
do you set foot on my\n\
imperial mountaintop?\n\
You may have eluded my\n\
guards, but you'll never\n\
escape my grasp...\n\
\n\
...and you'll never take\n\
away my Power Star. I\n\
hereby challenge you,\n\
Mario!\n\
If you want the Star I\n\
hold, you must prove\n\
yourself in battle.\n\
\n\
Can you pick me up from\n\
the back and hurl me to\n\
this royal turf? I think\n\
that you cannot!"))

//mouse diamon2 act1
DEFINE_DIALOG(DIALOG_018, 1, 4, 30, 200, _("\
That old shed is scary...\n\
I hear strange giggles.\n\
But the captain called\n\
me a scaredy rat..."))

//nipper talk act 1
DEFINE_DIALOG(DIALOG_019, 1, 2, 30, 200, _("\
Nip nip nip...\n\
I'm just vibing..."))

DEFINE_DIALOG(DIALOG_020, 1, 6, 95, 150, _("\
Dear Mario:\n\
Please come to the\n\
castle. I've baked\n\
a cake for you.\n\
Yours truly--\n\
Princess Toadstool"))

DEFINE_DIALOG(DIALOG_021, 1, 5, 95, 200, _("\
Welcome.\n\
No one's home!\n\
Now " SCRAM "\n\
and don't come back!\n\
Gwa ha ha!"))

//penguin act 3 smart
DEFINE_DIALOG(DIALOG_022, 1, 4, 95, 200, _("\
By my calculations,\n\
our penguin bodies are not\n\
suited to blow away\n\
that gate.\n\
It'd take the weight of\n\
a grown man to blow it\n\
up!"))

//penguin act 3 boss
DEFINE_DIALOG(DIALOG_023, 1, 2, 95, 200, _("\
No time to talk around,\n\
let's free the hound!\n\
Hop in the cannon\n\
already!\n\
Let's have that chomp\n\
chase Wario away!"))

//penguin act 3 sneaky
DEFINE_DIALOG(DIALOG_024, 1, 3, 95, 200, _("\
Wario is located!\n\
I'll keep tabs on him.\n\
Go get the chomp!"))

DEFINE_DIALOG(DIALOG_025, 1, 4, 95, 200, _("\
It takes the power of\n\
3 Stars to open this\n\
door. You need [%] more\n\
Stars."))

DEFINE_DIALOG(DIALOG_026, 1, 4, 95, 200, _("\
It takes the power of\n\
8 Stars to open this\n\
door. You need [%] more\n\
Stars."))

DEFINE_DIALOG(DIALOG_027, 1, 4, 95, 200, _("\
It takes the power of\n\
30 Stars to open this\n\
door. You need [%] more\n\
Stars."))

DEFINE_DIALOG(DIALOG_028, 1, 4, 95, 200, _("\
It takes the power of\n\
50 Stars to open this\n\
door. You need [%] more\n\
Stars."))

DEFINE_DIALOG(DIALOG_029, 1, 5, 95, 200, _("\
To open the door that\n\
leads to the 『endless』\n\
stairs, you need 70\n\
Stars.\n\
Bwa ha ha!"))

//mouse in OW
DEFINE_DIALOG(DIALOG_030, 1, 3, 30, 200, _("\
Our captain told us to\n\
mine here...\n\
\n\
Weird stuff is happening\n\
in this cave, but the\n\
gems are great!\n\
It'll take a while\n\
before we get back in."))

DEFINE_DIALOG(DIALOG_031, 1, 5, 30, 200, _("\
No way! You beat me...\n\
again!! And I just spent\n\
my entire savings on\n\
these new Koopa\n\
Mach 1 Sprint shoes!\n\
Here, I guess I have to\n\
hand over this Star to\n\
the winner of the race.\n\
Congrats, Mario!"))

//nipper talk act 1 guarding door
DEFINE_DIALOG(DIALOG_032, 1, 4, 30, 200, _("\
Hold on!\n\
Past this door is\n\
Nip - nip land!\n\
Only Nips allowed!"))

//captain act 2
DEFINE_DIALOG(DIALOG_033, 1, 4, 30, 200, _("\
Half my men went missing!\n\
You look fearless, champ...\n\
Could you give this boo\n\
shed a visit for ya pal?"))

//nipper act 2
DEFINE_DIALOG(DIALOG_034, 1, 3, 30, 200, _("\
Nip wonders...\n\
Where'd all the mice\n\
go to?\n\
Did a ghost eat them?"))


//savestate rap 1
DEFINE_DIALOG(DIALOG_035, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I come in peace,\n\
I mean no hate,\n\
But I've seen you\n\
using a savestate.\n\
Next time, think about\n\
not doing that please.\n\
I'll have no other option\n\
than to break your knees!"))

//savestate rap 2
DEFINE_DIALOG(DIALOG_036, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
Yo yo yoshi,\n\
I come from space\n\
I know you're a good guy,\n\
but just in case,\n\
savestates will soon\n\
ruin your fun.\n\
If you use 'em\n\
I've got a gun."))

DEFINE_DIALOG(DIALOG_037, 1, 2, 30, 200, _("\
I win! You lose!\n\
Ha ha ha ha!\n\
You're no slouch, but I'm\n\
a better sledder!\n\
Better luck next time!"))

DEFINE_DIALOG(DIALOG_038, 1, 3, 95, 200, _("\
Reacting to the Star\n\
power, the door slowly\n\
opens."))

//savestate rap 3
DEFINE_DIALOG(DIALOG_039, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
What's up,\n\
I come from far away.\n\
Using savestates\n\
is not okay.\n\
Your starcount is\n\
cheated and inflated.\n\
Do it again and\n\
you will be eviscerated."))

//savestate rap 4
DEFINE_DIALOG(DIALOG_040, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I'm state yoshi and\n\
I observe spacetime!\n\
I come to you with\n\
another rhyme!\n\
If I see on my radar\n\
that you've savestated...\n\
Your life privileges\n\
will be confiscated"))

DEFINE_DIALOG(DIALOG_041, 1, 3, 30, 200, _("\
I win! You lose!\n\
Ha ha ha!\n\
\n\
That's what you get for\n\
messin' with Koopa the\n\
Quick.\n\
Better luck next time!"))

//savestate rap 5
DEFINE_DIALOG(DIALOG_042, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I'm an alien yoshi,\n\
I'm immortal.\n\
I've come to you\n\
through a space portal.\n\
Savestateing would be\n\
a big blunder.\n\
Do it again and I'll...\n\
put you six feet under."))

//savestate rap 6
DEFINE_DIALOG(DIALOG_043, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I'm Yoshi from\n\
the time patrol.\n\
I watch over spacetime,\n\
that's my role.\n\
I've seen you savestate\n\
from inside a wormhole.\n\
If you keep doing it,\n\
I'll shut down the console."))

//wario zookeeper
DEFINE_DIALOG(DIALOG_044, 1, 5, 95, 200, _("\
Rotten day brother!\n\
There is a penguin\n\
outbreak in the Zoo.\n\
But I have more important\n\
things to do than you!\n\
Go catch 'em for me,\n\
will ya?\n\
I'll be at my house,\n\
make it quick or I'll\n\
kick you out, shrimp!\n\
GAHAHAHA"))
//wario running
DEFINE_DIALOG(DIALOG_045, 1, 3, 95, 200, _("\
This is rotten!\n\
Now the Chomp is\n\
free too??\n\
I'm out!"))

//savestate rap 7
DEFINE_DIALOG(DIALOG_046, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I'm a yoshi\n\
in a U F O.\n\
Savestates are bad,\n\
don't you know?\n\
I dont like people\n\
like you in my universe.\n\
Do it again and\n\
you'll be put in a hearse."))

DEFINE_DIALOG(DIALOG_047, 1, 2, 95, 200, _("\
Hi! I'll prepare the\n\
cannon for you!"))

//savestate rap 8
DEFINE_DIALOG(DIALOG_048, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I'm an alien yoshi\n\
from another dimension.\n\
I got something to say,\n\
I require your attention.\n\
If you savestate again,\n\
no matter the intention...\n\
Im gonna violate\n\
the Geneva convention."))

//savestate rap 9
DEFINE_DIALOG(DIALOG_049, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I'm a yoshi in a spaceship,\n\
I travel through the stars.\n\
I'm coming to your planet\n\
to spit some hot bars.\n\
Savestatin' ain't cool bro,\n\
I thought you knew that.\n\
If you keep on doing it,\n\
you'll become my doormat."))

//savestate rap 10
DEFINE_DIALOG(DIALOG_050, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
Space yoshi's rhymes\n\
are interplanetary.\n\
Better listen to me,\n\
I can get really scary.\n\
If you make me travel\n\
through another supernova,\n\
I'm gonna make sure\n\
that you game over."))

//savestate rap 11
DEFINE_DIALOG(DIALOG_051, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
What's up man,\n\
I'm space yoshi.\n\
I'll only say it once,\n\
so listen closely.\n\
If you savestate\n\
of your own will,\n\
I will send you\n\
straight to Brazil."))

//savestate rap 12
DEFINE_DIALOG(DIALOG_052, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I came all the way\n\
from the moon,\n\
so you better\n\
listen to my tune.\n\
Don't savestate,\n\
even once in a while\n\
or I'll sentence you\n\
to death without a trial."))

//savestate rap 13
DEFINE_DIALOG(DIALOG_053, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
I'm a yoshi\n\
in a satellite.\n\
I travel at the\n\
speed of light.\n\
I come to you\n\
with some advice.\n\
If you savestate,\n\
I'll shoot you twice!"))

//savestate rap 14
DEFINE_DIALOG(DIALOG_054, 1, 4, 30, 80, _("\
I've noticed a crack\n\
in the timeline,\n\
such an attack\n\
is not fine!\n\
Your savestates are rude,\n\
they ruin my mood\n\
So listen up dude,\n\
I have a feud:\n\
Savestate yoshi,\n\
that is my name.\n\
Spitting hot raps\n\
Is the reason I came.\n\
Savestate abusers?\n\
You losers are lame.\n\
Do it again and\n\
I will show you pain."))

// muscle penguin act 1
DEFINE_DIALOG(DIALOG_055, 1, 4, 30, 200, _("\
You friend?\n\
I throw you?\n\
\n\
//Throw// Don't Throw"))
//boss act 1
DEFINE_DIALOG(DIALOG_056, 1, 5, 30, 200, _("\
What's up my mammalian\n\
friend?\n\
You are looking to\n\
bring down that\n\
yellow twin of yours?\n\
Heh, you are in luck\n\
comrade!\n\
Me and my boys have\n\
the perfect plan.\n\
\n\
Just, one of us is\n\
currently being held\n\
in prison.\n\
\n\
\n\
If you can get him out,\n\
we'll bring down that\n\
yellow twin of yours,\n\
no problemo.\n\
\n\
My small friend here can\n\
hack the gates open for\n\
you!\n\
But don't run into any\n\
bob ombs, buddy!"))

//smart act 1
DEFINE_DIALOG(DIALOG_057, 1, 5, 30, 200, _("\
I've hacked the security\n\
system of this Zoo!\n\
If my leader trusts you,\n\
I can open the gate\n\
for you."))

//sneaky act 1
DEFINE_DIALOG(DIALOG_058, 1, 3, 30, 200, _("\
Hey boy!\n\
I'm a little bit stuck\n\
over here.\n\
Mind breaking this cage\n\
with that mighty behind\n\
of yours?"))

//sneaky act 1 text 2
DEFINE_DIALOG(DIALOG_059, 1, 3, 30, 200, _("\
Thanks boy!\n\
Here, I found this\n\
while sneaking around.\n\
It's useless to me,\n\
you can have it!"))

//bomb omb, military bob omb, opens cnnon
DEFINE_DIALOG(DIALOG_060, 1, 4, 30, 200, _("\
Careful young soldier!\n\
I can't let you use my\n\
cannon until you've\n\
proven your ability.\n\
Show me 15 stars or more\n\
and I'll open this up\n\
for you."))

//bomb omb, military bob omb, opens cnnon2
DEFINE_DIALOG(DIALOG_061, 1, 3, 30, 200, _("\
You've got 15\n\
Stars!!\n\
Nice one, soldier.\n\
I've got no doubts\n\
you are ready to use\n\
my cannon."))

//ask for safe file deletion
DEFINE_DIALOG(DIALOG_062, 1, 3, 30, 200, _("\
Do you want me to\n\
burn your old\n\
save game?\n\
Delete Save Game?\n\
\n\
//No Way!/ Yes"))

//mouse captain
DEFINE_DIALOG(DIALOG_063, 1, 4, 30, 200, _("\
Thanks for your help,\n\
champ!\n\
I would have been a snack\n\
without you!\n\
The nippers are now going\n\
crazy.\n\
My team had to go home.\n\
Good luck in these caves!"))

//cloud cap boxes
DEFINE_DIALOG(DIALOG_064, 1, 5, 30, 200, _("\
Once you've pressed\n\
the white switch\n\
in the Cloud Cap Palace,\n\
Mario can break this\n\
box to find a power-up!"))

//bomb box boxes
DEFINE_DIALOG(DIALOG_065, 1, 5, 30, 200, _("\
Once you've pressed\n\
the black switch\n\
in the Bomb Cap Palace,\n\
Mario can break this\n\
box to find a power-up!"))

//demo end
DEFINE_DIALOG(DIALOG_066, 1, 6, 30, 200, _("\
That is the end\n\
of the demo!\n\
I hope you had fun.\n\
Check out\n\
Kaze Emanuar\n\
on Youtube for more.\n\
The final game will have\n\
165 Stars.\n\
Be sure to follow\n\
the channel!\n\
\n\
\n\
Programming:\n\
Kaze Emanuar\n\
\n\
\n\
\n\
\n\
Game design:\n\
Kaze Emanuar\n\
\n\
\n\
\n\
\n\
Cinematics:\n\
Kaze Emanuar\n\
\n\
\n\
\n\
\n\
Music:\n\
Badub\n\
Frozen Eternal Flame\n\
Pabloscorner\n\
\n\
\n\
Tools:\n\
KureTheDead\n\
Decomp team\n\
\n\
\n\
\n\
Texture help:\n\
TheLegendOfZeina\n\
DanDrigues\n\
XPM\n\
Shbeebo\n\
\n\
Concept Art:\n\
TheLegendOfZeina\n\
DanDrigues\n\
Shbeebo\n\
\n\
\n\
Dialog:\n\
Kaze Emanuar\n\
Gravis"))

DEFINE_DIALOG(DIALOG_067, 1, 5, 30, 200, _("\
Tough luck, Mario!\n\
Princess Toadstool isn't\n\
here...Gwa ha ha!! Go\n\
ahead--just try to grab\n\
me by the tail!\n\
You'll never be able to\n\
swing ME around! A wimp\n\
like you won't throw me\n\
out of here! Never! Ha!"))
//waluigi text
DEFINE_DIALOG(DIALOG_068, 1, 2, 30, 200, _("\
This is Waluigi's Taco\n\
Stand!\n\
The Number One Tacos\n\
in the Mushroom Kingdom!\n\
But no Tacos for nosy\n\
little plumbers!\n\
Loser!\n\
Wahehehehe!"))
//shyguy ask buy
DEFINE_DIALOG(DIALOG_069, 1, 5, 30, 200, _("\
Welcome to the amazing\n\
Dorrie Snack Shop!\n\
The best way to befriend\n\
an aquatic dinosaur!\n\
\n\
Would you like to buy\n\
a Dorrie Snack for\n\
the low price of\n\
10 coins?\n\
//Buy/// Don't Buy"))

//shyguy good choice
DEFINE_DIALOG(DIALOG_070, 1, 2, 30, 200, _("\
Good choice my friend!\n\
Here, catch!"))

//shyguy you poor
DEFINE_DIALOG(DIALOG_071, 1, 2, 30, 200, _("\
You don't have enough\n\
coins!\n\
Stop wasting my time!"))
//shyguy you said no
DEFINE_DIALOG(DIALOG_072, 1, 1, 30, 200, _("\
Too bad!\n\
No money, no fun!"))
//leader pengu act 2
DEFINE_DIALOG(DIALOG_073, 1, 4, 95, 200, _("\
Hey recruit!\n\
You've been massively\n\
helpful already!\n\
\n\
I have another mission\n\
for you.\n\
\n\
\n\
There are engine parts\n\
all around the monkey cage.\n\
We'll need 5 of them.\n\
Could you sneak in?"))

//smart pengu act 2
DEFINE_DIALOG(DIALOG_074, 1, 3, 30, 200, _("\
We need parts\n\
to build ourselves\n\
a flying machine."))

//sneak pengu act 2
DEFINE_DIALOG(DIALOG_075, 1, 5, 30, 200, _("\
Thanks for the help\n\
earlier!\n\
By sneaking around,\n\
I've found the banana\n\
stash.\n\
That means phase 1 of\n\
our plan is complete.\n\
Talk to the boss for\n\
phase 2!"))
//wario talking to him open
DEFINE_DIALOG(DIALOG_076, 1, 3, 30, 200, _("\
Screw off!\n\
I don't want to see\n\
your nose around here!"))

//penguin muscle in aquarium
DEFINE_DIALOG(DIALOG_077, 1, 3, 150, 200, _("\
My friends come here to\n\
steal fish!\n\
I give them uppies!\n\
You want uppies?\n\
\n\
//Yes///  No"))
//wario garlic fish
DEFINE_DIALOG(DIALOG_078, 1, 4, 30, 200, _("\
Boring ocean fish.\n\
Maybe good with\n\
tartar sauce and\n\
garlic?"))
//jailukiki
DEFINE_DIALOG(DIALOG_079, 1, 5, 30, 200, _("\
Hey you!\n\
Be careful!\n\
Behind that door,\n\
there are tons of\n\
guards.\n\
If they spot you,\n\
they will explode\n\
right in your face!"))
//bosspengu in aquarium
DEFINE_DIALOG(DIALOG_080, 1, 1, 30, 200, _("\
Happy fishing, sport!"))

//sneakpengu in aquarium
DEFINE_DIALOG(DIALOG_081, 1, 2, 30, 200, _("\
Tehehe, this place\n\
is paradise!"))

//smartpengu in aquarium
DEFINE_DIALOG(DIALOG_082, 1, 4, 30, 200, _("\
By my calculations,\n\
the fish in here\n\
will feed us for\n\
years!"))
//orca statue
DEFINE_DIALOG(DIALOG_083, 1, 2, 30, 200, _("\
This is an orca statue.\n\
Fun orca fact:\n\
You smell!"))
//mario fish statue
DEFINE_DIALOG(DIALOG_084, 1, 2, 30, 200, _("\
A bunch of\n\
mean fish!"))
//submarine
DEFINE_DIALOG(DIALOG_085, 1, 2, 30, 200, _("\
Submarine.\n\
\n\
Good for diving\n\
for treasure!"))
//unagi
DEFINE_DIALOG(DIALOG_086, 1, 1, 30, 200, _("\
Very mean fish!"))
//bloopers
DEFINE_DIALOG(DIALOG_087, 1, 1, 30, 200, _("\
Annoying things!"))
//wind fish
DEFINE_DIALOG(DIALOG_088, 1, 1, 30, 200, _("\
Mystical wind fish.\n\
He's really hideous!"))
// muscle penguin act 4,5,6
DEFINE_DIALOG(DIALOG_089, 1, 4, 30, 200, _("\
You friend!\n\
I throw you?\n\
\n\
//Throw// Don't Throw"))

//intro text from bowser
DEFINE_DIALOG(DIALOG_090, 1, 4, 30, 200, _("\
Bwahahaha!\n\
Those stupid dinosaurs\n\
have no idea what is\n\
about to hit them!"))
//boss penguin act 4,5,6
DEFINE_DIALOG(DIALOG_091, 1, 3, 30, 200, _("\
Flightless birds?\n\
No more!\n\
\n\
Our little smarty here\n\
came up with an\n\
ingenious machine!"))

//2nd intro text from bowser
DEFINE_DIALOG(DIALOG_092, 1, 5, 30, 200, _("\
That trick keeps working!\n\
Now I've captured all of\n\
these stupid Yoshis!\n\
Time to make them work!\n\
Bwahahaha!"))
//sneaky penguin act 4,5,6
DEFINE_DIALOG(DIALOG_093, 1, 3, 30, 200, _("\
Hehehe, without\n\
that Wario surveilling\n\
us, we're finally free!"))
//smart penguin act 4,5,6
DEFINE_DIALOG(DIALOG_094, 1, 3, 30, 200, _("\
All my dreams are\n\
in this machine!\n\
\n\
I've done it!\n\
Antarctica, here we\n\
come!"))
//UNAGI JOKE 1
DEFINE_DIALOG(DIALOG_095, 1, 2, 150, 150, _("\
Welcome to the\n\
Unagi Comedy Club!\n\
Hope you all are in the\n\
mood for great comedy!\n\
Why does the green Toad\n\
set his alarm to 4AM?\n\
Ready?...\n\
\n\
...\n\
...\n\
SO HE GETS TO BE THE\n\
FIRST 1-UP!!\n\
BAHAHAHAHA!"))

//UNAGI JOKE 2
DEFINE_DIALOG(DIALOG_096, 1, 2, 150, 150, _("\
Welcome back to the\n\
Unagi Comedy Club!\n\
It's time for some\n\
great comedy!\n\
Some people say they\n\
hate my jokes!\n\
Wanna know what I\n\
think about that?\n\
...\n\
...\n\
They just can't\n\
D - eel with my puns!\n\
BAHAHAHAHA!"))
//UNAGI JOKE 3
DEFINE_DIALOG(DIALOG_097, 1, 2, 150, 150, _("\
Welcome back to the\n\
Unagi Comedy Club!\n\
It's time for some\n\
great comedy!\n\
Why is the underwater\n\
economy so bad?\n\
Does anybody\n\
know?\n\
...\n\
...\n\
It's because everyone\n\
is so Cheep - Cheep!\n\
BAHAHAHAHA!"))
//labyrinth sign
DEFINE_DIALOG(DIALOG_098, 1, 2, 95, 200, _("\
Entry to the\n\
Captain's Labyrinth.\n\
Careful, you might fall\n\
into an early grave."))

// shyguy captains log
DEFINE_DIALOG(DIALOG_099, 1, 6, 95, 200, _("\
Captain's Log:\n\
The Shying Dutchman\n\
is floating as strong\n\
as ever!\n\
\n\
\n\
We are still stuck\n\
in this doomed\n\
ocean, but we\n\
have a great time\n\
at sea!\n\
\n\
Whenever we see\n\
another ship, we\n\
take their loot!\n\
I'm just sick\n\
of counting all the\n\
money we made.\n\
I think I've\n\
dropped a piece\n\
of treassure under\n\
my table, let\n\
the bandana guys\n\
clean that up."))

DEFINE_DIALOG(DIALOG_100, 1, 3, 95, 200, _("\
Ukkiki...Wakkiki...kee kee!\n\
Ha! I snagged it!\n\
It's mine! Heeheeheeee!"))

DEFINE_DIALOG(DIALOG_101, 1, 3, 95, 200, _("\
Ackk! Let...go...\n\
You're...choking...me...\n\
Cough...I've been framed!\n\
This Cap? Oh, all right,\n\
take it. It's a cool Cap,\n\
but I'll give it back.\n\
I think it looks better on\n\
me than it does on you,\n\
though! Eeeee! Kee keee!"))

//UNAGI JOKE 4
DEFINE_DIALOG(DIALOG_102, 1, 2, 150, 150, _("\
Welcome back to the\n\
Unagi Comedy Club!\n\
It's time for some\n\
great comedy!\n\
A friend of mine claims\n\
that he is electric.\n\
Wanna know what happened\n\
when I found out?\n\
...\n\
...\n\
I was shocked!\n\
\n\
BAHAHAHAHA!"))

//UNAGI JOKE 5
DEFINE_DIALOG(DIALOG_103, 1, 2, 150, 150, _("\
Welcome back to the\n\
Unagi Comedy Club!\n\
It's time for some\n\
great comedy!\n\
Filming my specials\n\
is such a hassle.\n\
Something always goes\n\
wrong, wanna know why?\n\
...\n\
...\n\
Because there are\n\
too many Bloopers!\n\
BAHAHAHAHA!"))

//UNAGI JOKE 6
DEFINE_DIALOG(DIALOG_104, 1, 2, 150, 150, _("\
Welcome back to the\n\
Unagi Comedy Club!\n\
This is my last act\n\
for the day!\n\
How can you break those\n\
tough blocks?\n\
Don't you know?\n\
You can't guess it?\n\
...\n\
...\n\
Well, use your head, \n\
dummy!\n\
BAHAHAHAHA!"))

DEFINE_DIALOG(DIALOG_105, 1, 3, 95, 200, _("\
Ready for blastoff! Come\n\
on, hop into the cannon!\n\
\n\
You can reach the Star on\n\
the floating island by\n\
using the four cannons.\n\
Use the Control Stick to\n\
aim, then press [A] to fire.\n\
\n\
If you're handy, you can\n\
grab on to trees or poles\n\
to land."))

DEFINE_DIALOG(DIALOG_106, 1, 2, 95, 200, _("\
Ready for blastoff! Come\n\
on, hop into the cannon!"))

//shyguy says init
DEFINE_DIALOG(DIALOG_107, 1, 2, 95, 200, _("\
Hey Landlubber, aren't\n\
you afraid of ghosts?\n\
There's a game we\n\
pirates love to play.\n\
It's called\n\
Shyguy - Says.\n\
Follow my lead,\n\
if you die, you lose!"))

//shyguy says win
DEFINE_DIALOG(DIALOG_108, 1, 4, 95, 200, _("\
Great job, matey!\n\
You'd make for a\n\
great pirate.\n\
Take your prize!"))
//lonely shyguy
DEFINE_DIALOG(DIALOG_109, 1, 4, 95, 200, _("\
This plunder expedition\n\
blows!\n\
Noone wants to be\n\
my friend!\n\
I thought plundering was\n\
going to be fun!\n\
Never again!"))
//shyguy pirate longing for girl
DEFINE_DIALOG(DIALOG_110, 1, 3, 95, 200, _("\
Oh how I miss my\n\
Shyreen!\n\
\n\
We've gotten so\n\
close over the\n\
years!\n\
She almost let me\n\
see her with no mask!"))
//shyguy captain text 1
DEFINE_DIALOG(DIALOG_111, 1, 3, 95, 200, _("\
Who arrrre you?!\n\
Arrrrre you after me\n\
booty?\n\
You shalt perrrrish\n\
by my sworrrrrd!"))
//yoshi lazing off easteregg 1
DEFINE_DIALOG(DIALOG_112, 1, 4, 30, 200, _("\
That douche tricked us!\n\
I hate him!\n\
I'll vandalize his whole\n\
hideout!!"))
//yoshi lazing off easteregg 2
DEFINE_DIALOG(DIALOG_113, 1, 3, 30, 200, _("\
Heh, that stupid turtle\n\
thought he could make me\n\
work for him!\n\
I escaped and now I'm\n\
having fun!"))
//yoshimama
DEFINE_DIALOG(DIALOG_114, 1, 3, 95, 200, _("\
I keep the egg warm!\n\
But those 3 goons are\n\
giving me the creeps!"))
//dipshit 2 talking
DEFINE_DIALOG(DIALOG_115, 1, 3, 95, 200, _("\
Stop snooping, chump!\n\
Can't you see we're\n\
busy?"))
//spike on dipshit exposure
DEFINE_DIALOG(DIALOG_116, 1, 2, 95, 200, _("\
Mission failed, boys!\n\
Hightail it!"))

DEFINE_DIALOG(DIALOG_117, 1, 1, 95, 200, _("\
Who...walk...here?\n\
Who...break...seal?\n\
Wake..ancient..ones?\n\
We no like light...\n\
Rrrrummbbble...\n\
We no like...intruders!\n\
Now battle...\n\
...hand...\n\
...to...\n\
...hand!"))

DEFINE_DIALOG(DIALOG_118, 1, 6, 95, 200, _("\
Grrrrumbbble!\n\
What...happen?\n\
We...crushed like pebble.\n\
You so strong!\n\
You rule ancient pyramid!\n\
For today...\n\
Now, take Star of Power.\n\
We...sleep...darkness."))
//bowser boss 1
DEFINE_DIALOG(DIALOG_119, 1, 2, 30, 200, _("\
Bahaha you fell right\n\
into my trap!\n\
Get ready to face my\n\
new creation - Mecha-Me!\n\
Tough luck, I'm out\n\
of here!"))

DEFINE_DIALOG(DIALOG_120, 1, 4, 30, 200, _("\
Ooowaah! Can it be that\n\
I've lost??? The power of\n\
the Stars has failed me...\n\
this time.\n\
Consider this a draw.\n\
Next time, I'll be in\n\
perfect condition.\n\
\n\
Now, if you want to see\n\
your precious Princess,\n\
come to the top of the\n\
tower.\n\
I'll be waiting!\n\
Gwa ha ha ha!"))

DEFINE_DIALOG(DIALOG_121, 1, 5, 30, 200, _("\
Nooo! It can't be!\n\
You've really beaten me,\n\
Mario?!! I gave those\n\
troops power, but now\n\
it's fading away!\n\
Arrgghh! I can see peace\n\
returning to the world! I\n\
can't stand it! Hmmm...\n\
It's not over yet...\n\
\n\
C'mon troops! Let's watch\n\
the ending together!\n\
Bwa ha ha!"))
//penguinboss star
DEFINE_DIALOG(DIALOG_122, 1, 3, 30, 200, _("\
Great job soldier!\n\
You've earned a\n\
reward!"))
//yoshi dipshits giving star
DEFINE_DIALOG(DIALOG_123, 1, 2, 30, 200, _("\
Wah!\n\
\n\
We've been scrambled,\n\
boys!"))
//yoshi spike after entering area
DEFINE_DIALOG(DIALOG_124, 1, 2, 30, 200, _("\
He's following us!\n\
Take care boys!"))

DEFINE_DIALOG(DIALOG_125, 1, 3, 30, 200, _("\
Hazy Maze-Exit\n\
Danger! Closed.\n\
Turn back now."))

DEFINE_DIALOG(DIALOG_126, 2, 3, 30, 200, _("\
Up: Black Hole\n\
Right: Work Elevator\n\
/// Hazy Maze"))

//boonard chit chat
DEFINE_DIALOG(DIALOG_127, 1, 4, 30, 200, _("\
You came back, buddy!\n\
Care to join for tea?\n\
I have lots more stories\n\
to tell!"))

DEFINE_DIALOG(DIALOG_128, 1, 4, 95, 200, _("\
You must fight with\n\
honor! It is against the\n\
royal rules to throw the\n\
king out of the ring!"))

DEFINE_DIALOG(DIALOG_129, 1, 5, 30, 200, _("\
Welcome to the Vanish\n\
Cap Switch Course! All of\n\
the blue blocks you find\n\
will become solid once you\n\
step on the Cap Switch.\n\
You'll disappear when you\n\
put on the Vanish Cap, so\n\
you'll be able to elude\n\
enemies and walk through\n\
many things. Try it out!"))

//mouse captain thanks for saving
DEFINE_DIALOG(DIALOG_130, 1, 4, 30, 200, _("\
You saved my crew\n\
from that nest!\n\
Great, now they can\n\
continue mining!"))

//boonard beat piranha
DEFINE_DIALOG(DIALOG_131, 1, 5, 30, 200, _("\
Good fight, pal!\n\
That's enough action\n\
for an old geezer\n\
like me for\n\
a whole century!\n\
I'll be retiring!"))

//muscle pengu act 2
DEFINE_DIALOG(DIALOG_132, 1, 3, 30, 200, _("\
My friend send you?\n\
Me throw?\n\
//Yes///  No"))

//boonard found tnt
DEFINE_DIALOG(DIALOG_133, 1, 3, 30, 200, _("\
I found this TNT!\n\
The mice used it to\n\
dig in the cave.\n\
Make the piranha\n\
eat this instead!"))

//boonard boss
DEFINE_DIALOG(DIALOG_134, 1, 4, 30, 200, _("\
Did you see that cage?\n\
All the mice were in\n\
there!\n\
\n\
The Nippers want to\n\
feed it to their\n\
Mama Piranha!\n\
\n\
I am going to pull\n\
one last prank.\n\
Survive for a bit\n\
and wait for my help!"))

//boonard entry act 3
DEFINE_DIALOG(DIALOG_135, 1, 4, 30, 200, _("\
You came back!\n\
I've taken a liking\n\
to you and want to help\n\
you rescue those mice.\n\
Let me tag along.\n\
My mirror image\n\
can pick stuff up\n\
for you!"))

//boonard entry
DEFINE_DIALOG(DIALOG_136, 1, 4, 30, 200, _("\
Oh, a visitor?\n\
Never thought someone\n\
would come to visit\n\
an old geezer like me!\n\
What brought you here?\n\
...\n\
Missing mice?\n\
\n\
Haha, if I was 400 years\n\
younger I might,\n\
but my pranking days\n\
are long over!\n\
Why don't you look\n\
through my house\n\
and convince yourself?"))

DEFINE_DIALOG(DIALOG_137, 1, 6, 30, 200, _("\
Thanks, Mario! The castle\n\
is recovering its energy\n\
as you retrieve Power\n\
Stars, and you've chased\n\
Bowser right out of here,\n\
on to some area ahead.\n\
Oh, by the by, are you\n\
collecting coins? Special\n\
Stars appear when you\n\
collect 100 coins in each\n\
of the 15 courses!"))

//boonard no story
DEFINE_DIALOG(DIALOG_138, 1, 3, 30, 200, _("\
Hm? You what?\n\
Confounded cheek!\n\
\n\
Refusing to reciprocate\n\
my recital of rare\n\
romance, how very rude!"))

//boonard yes story
DEFINE_DIALOG(DIALOG_139, 1, 4, 30, 200, _("\
Ah, splendid! Perhaps\n\
you whippersnappers\n\
aren't so bad after all.\n\
Ehem!\n\
The tale I have to tell\n\
details my demure\n\
damsel, my bonnie bride,\n\
my beautiful Booatrice.\n\
Ah, I remember it like\n\
it was only a few\n\
centuries ago...\n\
Ah, we were so young...\n\
We arranged to get\n\
married on the 5th of\n\
Quintilis, in secret, so as\n\
not to upset our families.\n\
I came from the house\n\
of Prydon, she from\n\
Patrexis. Such a marriage\n\
was scandalous back then!\n\
I remember, during the\n\
wedding, a man with a\n\
hunched back came in,\n\
and tried to stop us.\n\
He was shouting about\n\
how he'd come back in\n\
time, to stop the wedding\n\
from taking place!\n\
And that's how we ended\n\
up in the future, stopping\n\
the Quatch invasion of\n\
Earth, in 2150AD!\n\
Long story short, I\n\
turned around, and they\n\
were all wearing eye\n\
patches!\n\
What a jolly jape!\n\
It was quite the wedding\n\
day, let me tell you sir!"))

//boonard thanks for listening
DEFINE_DIALOG(DIALOG_140, 1, 3, 30, 200, _("\
That's all I have to say\n\
I'm afraid. I'm... sorry I\n\
was rude to you earlier.\n\
I've been on my own for\n\
so long, I forgot how nice\n\
it was to have company.\n\
If you ever come here\n\
again, I can help you\n\
find the nipper's nest.\n\
Oh, one last thing, take\n\
this, as a token of an\n\
old Boo's gratitude!"))

DEFINE_DIALOG(DIALOG_141, 1, 5, 150, 200, _("\
You've recovered one of\n\
the stolen Power Stars!\n\
Now you can open some of\n\
the sealed doors in the\n\
castle.\n\
Try the Princess's room\n\
on the second floor and\n\
the room with the\n\
painting of Whomp's\n\
Fortress on Floor 1.\n\
Bowser's troops are still\n\
gaining power, so you\n\
can't give up. Save us,\n\
Mario! Keep searching for\n\
Stars!"))

DEFINE_DIALOG(DIALOG_142, 1, 5, 150, 200, _("\
You've recovered three\n\
Power Stars! Now you can\n\
open any door with a 3\n\
on its star.\n\
\n\
You can come and go from\n\
the open courses as you\n\
please. The enemies ahead\n\
are even meaner, so be\n\
careful!"))

DEFINE_DIALOG(DIALOG_143, 1, 6, 150, 200, _("\
You've recovered eight of\n\
the Power Stars! Now you\n\
can open the door with\n\
the big Star! But Bowser\n\
is just ahead...can you\n\
hear the Princess calling?"))

DEFINE_DIALOG(DIALOG_144, 1, 6, 150, 200, _("\
You've recovered 30\n\
Power Stars! Now you can\n\
open the door with the\n\
big Star! But before you\n\
move on, how's it going\n\
otherwise?\n\
Did you pound the two\n\
columns down? You didn't\n\
lose your hat, did you?\n\
If you did, you'll have to\n\
stomp on the condor to\n\
get it back!\n\
They say that Bowser has\n\
sneaked out of the sea\n\
and into the underground.\n\
Have you finally\n\
cornered him?"))

DEFINE_DIALOG(DIALOG_145, 1, 6, 150, 200, _("\
You've recovered 50\n\
Power Stars! Now you can\n\
open the Star Door on the\n\
third floor. Bowser's\n\
there, you know.\n\
\n\
Oh! You've found all of\n\
the Cap Switches, haven't\n\
you? Red, green and blue?\n\
The Caps you get from the\n\
colored blocks are really\n\
helpful.\n\
Hurry along, now. The\n\
third floor is just ahead."))

DEFINE_DIALOG(DIALOG_146, 1, 6, 150, 200, _("\
You've found 70 Power\n\
Stars! The mystery of the\n\
endless stairs is solved,\n\
thanks to you--and is\n\
Bowser ever upset! Now,\n\
on to the final bout!"))

//boonard free chair, invites for tea
DEFINE_DIALOG(DIALOG_147, 1, 6, 30, 200, _("\
Hm? Yes? What is it?\n\
What do you want, hm?\n\
Well, out with it young\n\
man! The impertinance\n\
of youth knows no bounds!\n\
\n\
Oh, very well, if you\n\
insist on staying, I had\n\
best offer you some light\n\
refreshment. I am still\n\
a gentleman after all.\n\
\n\
While the tea's being\n\
brewed, shall I regale you\n\
with some tall tales from\n\
the talisman of my time\n\
as a tyke like you, hm?\n\
//Yes/// No"))


//boonard tax returns
DEFINE_DIALOG(DIALOG_148, 1, 5, 30, 200, _("\
Those are my taxes!\n\
It sounds silly, but doing\n\
my taxes is one of my\n\
favourite ways of passing\n\
the millennia.\n\
If you ever need help\n\
with yours, I would be\n\
happy to be your\n\
tax ghostwriter!\n\
\n\
There's something about\n\
being alone, just doing\n\
the same repetitive task\n\
over and over that's so...\n\
Soothing.\n\
It's a pity that since the\n\
economy collapsed all I\n\
can afford is a small\n\
old shed in a cave, but\n\
easy come easy go."))

//boonard son
DEFINE_DIALOG(DIALOG_149, 1, 4, 30, 200, _("\
Be careful with that!\n\
That's a picture of my\n\
son, not a child's toy!\n\
\n\
Ah, he was a good Boo,\n\
He always used to play\n\
on his Game Boo Colour.\n\
\n\
He's moved out now...\n\
I miss him dreadfully...\n\
I feel ghosted by him\n\
sometimes..."))

DEFINE_DIALOG(DIALOG_150, 1, 5, 30, 200, _("\
Waaaa! You've flooded my\n\
house! Wh-why?? Look at\n\
this mess! What am I\n\
going to do now?\n\
\n\
The ceiling's ruined, the\n\
floor is soaked...what to\n\
do, what to do? Huff...\n\
huff...it makes me so...\n\
MAD!!!\n\
Everything's been going\n\
wrong ever since I got\n\
this Star...It's so shiny,\n\
but it makes me feel...\n\
strange..."))

DEFINE_DIALOG(DIALOG_151, 1, 4, 30, 200, _("\
I can't take this\n\
anymore! First you get\n\
me all wet, then you\n\
stomp on me!\n\
Now I'm really, really,\n\
REALLY mad!\n\
Waaaaaaaaaaaaaaaaa!!!"))

DEFINE_DIALOG(DIALOG_152, 1, 3, 30, 200, _("\
Owwch! Uncle! Uncle!\n\
Okay, I " GIVE_UP ". Take this\n\
Star!\n\
Whew! I feel better now.\n\
I don't really need it\n\
anymore, anyway--\n\
I can see the stars\n\
through my ceiling at\n\
night.\n\
They make me feel...\n\
...peaceful. Please, come\n\
back and visit anytime."))

//boonard family painting wife
DEFINE_DIALOG(DIALOG_153, 1, 5, 30, 200, _("\
My dear wife Booatrice.\n\
I love my Boo very much.\n\
I'm a very lucky Boo to\n\
have her. She's really\n\
quite shy, you know.\n\
We once took a trip, and\n\
dash it all if she wasn't\n\
stuck in place, too scared\n\
to move, because of, of\n\
all things, a statue!\n\
We had to cover it with\n\
a sheet so that she could\n\
move on, but of course\n\
some children got scared,\n\
thinking it was a ghost!\n\
A ghost in a museum!\n\
Who could imagine such\n\
a thing?"))

//boonard goul medal
DEFINE_DIALOG(DIALOG_154, 1, 4, 30, 200, _("\
Ah, I see you've found my\n\
Ghoul Medal. I got this\n\
back when I was a young\n\
boo, about 450 or so.\n\
I got it during one of the\n\
Scare Competitions, and\n\
ah, what fun those games\n\
used to be...\n\
We used to hide in an\n\
old Mansion, and try to\n\
scare anyone who came\n\
in! Tremendous fun!\n\
Of course, I'm much too\n\
old for all that now.\n\
Only one thing is scary\n\
about me now...\n\
The ravages of old age!\n\
Muwahahahahahahaha!"))

//goombaking
DEFINE_DIALOG(DIALOG_155, 1, 3, 30, 200, _("\
You've beaten me...\n\
but my troops are\n\
infinite...\n\
The Goombas will\n\
have their revenge\n\
on you!"))
//goombaking
DEFINE_DIALOG(DIALOG_156, 1, 4, 30, 200, _("\
You worm were hiding\n\
on my crown!\n\
What a cowardly tactic!\n\
\n\
But a lightweight like\n\
you would never hurt\n\
me just by standing\n\
there!\n\
You'll need more force\n\
than that!"))

//goombaking
DEFINE_DIALOG(DIALOG_157, 1, 3, 30, 200, _("\
I've been waiting\n\
to ambush you!\n\
\n\
You stopped my troops\n\
from their food\n\
gathering mission!\n\
As King of the Goomba,\n\
I shall hereby\n\
punish you, worm!"))

//bunny celebrating
DEFINE_DIALOG(DIALOG_158, 1, 3, 30, 200, _("\
The plaza is free again!\n\
We can celebrate carrots\n\
after all!\n\
You are my hero!"))

//bunny thanks for saving
DEFINE_DIALOG(DIALOG_159, 1, 3, 30, 200, _("\
You showed that Goomba!\n\
You should be granted\n\
honorary bunnyship!"))

//bunny just talking in town
DEFINE_DIALOG(DIALOG_160, 1, 5, 30, 200, _("\
This is the town plaza!\n\
We'd be celebrating\n\
the annual carrot festival\n\
now, but the Goombas\n\
keep pestering us."))

//bunny just talking
DEFINE_DIALOG(DIALOG_161, 1, 3, 30, 200, _("\
We build our burrows in\n\
carved out trees like\n\
this one!\n\
Except mine is a slide!"))

//bunny just talking
DEFINE_DIALOG(DIALOG_162, 1, 3, 30, 200, _("\
Something fell onto my\n\
house!\n\
What could it be?"))

//bunny just talking
DEFINE_DIALOG(DIALOG_163, 1, 4, 30, 200, _("\
Ever tried to do a dive\n\
during a groundpound?\n\
It lets you do a cool\n\
mid - air jump!"))

//bunny need help in storage room
DEFINE_DIALOG(DIALOG_164, 1, 4, 30, 200, _("\
Our storage room got\n\
invaded!\n\
Those Goombas won't\n\
leave us alone.\n\
You look like you've\n\
jumped on some\n\
Goombas before.\n\
Would you help us out?"))

//sign3
DEFINE_DIALOG(DIALOG_165, 1, 3, 30, 200, _("\
The lowest of\n\
the pillars."))
//sign2
DEFINE_DIALOG(DIALOG_166, 1, 4, 30, 200, _("\
Follow my lead:\n\
Blue, Pink, Purple,\n\
Orange, Red, Green,\n\
Yellow!\n\
And don't leave the\n\
hexagons!"))
//sign1
DEFINE_DIALOG(DIALOG_167, 1, 4, 30, 200, _("\
The smaller\n\
flower field."))
//sign0
DEFINE_DIALOG(DIALOG_168, 1, 4, 30, 200, _("\
Yargh Matey!\n\
There is a treasure\n\
map hidden on my\n\
Ghostly ship.\n\
Would you like to find\n\
it?\n\
Check the tip of my ship!"))

DEFINE_DIALOG(DIALOG_169, 1, 5, 30, 200, _("\
Did you know?\n\
You can press B\n\
during a groundpound\n\
to do a big dive\n\
in any direction!"))

//yoshitexts
DEFINE_DIALOG(DIALOG_170, 1, 4, 30, 200, _("\
How could you wake me!\n\
I was just dreaming of an\n\
extra large honey melon!!\n\
...\n\
Oh right, you came to\n\
save the Yoshi clan.\n\
I'd love to help, but\n\
I've a naptime scheduled..."))
DEFINE_DIALOG(DIALOG_171, 1, 4, 30, 200, _("\
Wow you really freed\n\
my friends!\n\
There are still many\n\
Yoshis left though.\n\
Don't get lazy.\n\
Unlike me.\n\
I need another nap."))
DEFINE_DIALOG(DIALOG_172, 1, 5, 30, 200, _("\
Being this lazy takes\n\
serious talent.\n\
I bet you couldn't do it.\n\
\n\
\n\
You couldn't jump around\n\
as much if you were as\n\
lazy as I am!!\n\
\n\
\n\
You think you can do it?\n\
Prove it!\n\
Get the red coins in\n\
Bunny Brushwoods\n\
with 2 or fewer jumps!"))
DEFINE_DIALOG(DIALOG_173, 1, 4, 30, 200, _("\
Lazyyosh star 2"))
DEFINE_DIALOG(DIALOG_174, 1, 4, 30, 200, _("\
Lazyyosh star 3"))
DEFINE_DIALOG(DIALOG_175, 1, 4, 30, 200, _("\
Lazyyosh star 4"))


DEFINE_DIALOG(DIALOG_176, 1, 2, 30, 200, _("\
Thanks for saving me,\n\
buddy!"))
DEFINE_DIALOG(DIALOG_177, 1, 4, 30, 200, _("\
Greenyosh star1"))
DEFINE_DIALOG(DIALOG_178, 1, 4, 30, 200, _("\
Greenyosh star2"))
DEFINE_DIALOG(DIALOG_179, 1, 4, 30, 200, _("\
Greenyosh star3"))
DEFINE_DIALOG(DIALOG_180, 1, 4, 30, 200, _("\
Greenyosh star4"))

DEFINE_DIALOG(DIALOG_181, 1, 3, 30, 200, _("\
Tremendous endeavor, son.\n\
We were unshackled,\n\
once again.\n\
What a pleasureable\n\
sunset to see."))
DEFINE_DIALOG(DIALOG_182, 1, 4, 30, 200, _("\
Blackyosh star1"))
DEFINE_DIALOG(DIALOG_183, 1, 4, 30, 200, _("\
Blackyosh star2"))
DEFINE_DIALOG(DIALOG_184, 1, 4, 30, 200, _("\
Blackyosh star3"))
DEFINE_DIALOG(DIALOG_185, 1, 4, 30, 200, _("\
Blackyosh star4"))

DEFINE_DIALOG(DIALOG_186, 1, 2, 30, 200, _("\
Hmm.\n\
Yes, this is better.\n\
It was very warm\n\
in there."))
DEFINE_DIALOG(DIALOG_187, 1, 4, 30, 200, _("\
Blueyosh star1"))
DEFINE_DIALOG(DIALOG_188, 1, 4, 30, 200, _("\
Blueyosh star2"))
DEFINE_DIALOG(DIALOG_189, 1, 4, 30, 200, _("\
Blueyosh star3"))
DEFINE_DIALOG(DIALOG_190, 1, 4, 30, 200, _("\
Blueyosh star4"))

DEFINE_DIALOG(DIALOG_191, 1, 2, 30, 200, _("\
Waaah!\n\
I was so scared!\n\
You are the best,\n\
Mario!!"))
DEFINE_DIALOG(DIALOG_192, 1, 4, 30, 200, _("\
Cyanyosh star1"))
DEFINE_DIALOG(DIALOG_193, 1, 4, 30, 200, _("\
Cyanyosh star2"))
DEFINE_DIALOG(DIALOG_194, 1, 4, 30, 200, _("\
Cyanyosh star3"))
DEFINE_DIALOG(DIALOG_195, 1, 4, 30, 200, _("\
Cyanyosh star4"))

DEFINE_DIALOG(DIALOG_196, 1, 2, 30, 200, _("\
Heh, I totally slacked\n\
off in those mines!\n\
And I got free food!\n\
Hehe."))
DEFINE_DIALOG(DIALOG_197, 1, 4, 30, 200, _("\
Brownyosh star1"))
DEFINE_DIALOG(DIALOG_198, 1, 4, 30, 200, _("\
Brownyosh star2"))
DEFINE_DIALOG(DIALOG_199, 1, 4, 30, 200, _("\
Brownyosh star3"))
DEFINE_DIALOG(DIALOG_200, 1, 4, 30, 200, _("\
Brownyosh star4"))

DEFINE_DIALOG(DIALOG_201, 1, 3, 30, 200, _("\
It's too cold!\n\
At least it was warm in\n\
the mines!\n\
If only that meanie\n\
wasn't there!\n\
\n\
Wah!\n\
Nothing is comfy!"))
DEFINE_DIALOG(DIALOG_202, 1, 4, 30, 200, _("\
Pinkyosh star1"))
DEFINE_DIALOG(DIALOG_203, 1, 4, 30, 200, _("\
Pinkyosh star2"))
DEFINE_DIALOG(DIALOG_204, 1, 4, 30, 200, _("\
Pinkyosh star3"))
DEFINE_DIALOG(DIALOG_205, 1, 4, 30, 200, _("\
Pinkyosh star4"))

DEFINE_DIALOG(DIALOG_206, 1, 2, 30, 200, _("\
Your rescue mission\n\
was fabulous, darlinkie!"))
DEFINE_DIALOG(DIALOG_207, 1, 4, 30, 200, _("\
Purpleyosh star1"))
DEFINE_DIALOG(DIALOG_208, 1, 4, 30, 200, _("\
Purpleyosh star2"))
DEFINE_DIALOG(DIALOG_209, 1, 4, 30, 200, _("\
Purpleyosh star3"))
DEFINE_DIALOG(DIALOG_210, 1, 4, 30, 200, _("\
Purpleyosh star4"))

DEFINE_DIALOG(DIALOG_211, 1, 2, 30, 200, _("\
I hate Bowser!\n\
I hate Bowser!\n\
I hate him so much!\n\
He will regret that!"))
DEFINE_DIALOG(DIALOG_212, 1, 4, 30, 200, _("\
Redyosh star1"))
DEFINE_DIALOG(DIALOG_213, 1, 4, 30, 200, _("\
Redyosh star2"))
DEFINE_DIALOG(DIALOG_214, 1, 4, 30, 200, _("\
Redyosh star3"))
DEFINE_DIALOG(DIALOG_215, 1, 4, 30, 200, _("\
Redyosh star4"))

DEFINE_DIALOG(DIALOG_216, 1, 1, 30, 200, _("\
...Thank you..."))
DEFINE_DIALOG(DIALOG_217, 1, 4, 30, 200, _("\
Whiteyosh star1"))
DEFINE_DIALOG(DIALOG_218, 1, 4, 30, 200, _("\
Whiteyosh star2"))
DEFINE_DIALOG(DIALOG_219, 1, 4, 30, 200, _("\
Whiteyosh star3"))
DEFINE_DIALOG(DIALOG_220, 1, 4, 30, 200, _("\
Whiteyosh star4"))

DEFINE_DIALOG(DIALOG_221, 1, 3, 30, 200, _("\
Yay!\n\
You freed us!\n\
You are the best!"))
DEFINE_DIALOG(DIALOG_222, 1, 4, 30, 200, _("\
yellowyopsh star1"))
DEFINE_DIALOG(DIALOG_223, 1, 4, 30, 200, _("\
yellowyopsh star2"))
DEFINE_DIALOG(DIALOG_224, 1, 4, 30, 200, _("\
yellowyopsh star3"))
DEFINE_DIALOG(DIALOG_225, 1, 4, 30, 200, _("\
yellowyopsh star4"))

DEFINE_DIALOG(DIALOG_226, 1, 4, 30, 200, _("\
Yoshi make Mario go wee,\n\
and then Yoshi say bye,\n\
Mario will fly to the sky!\n\
//Yes/// No"))
DEFINE_DIALOG(DIALOG_227, 1, 2, 30, 200, _("\
Don't be a dummy,\n\
bounce off my tummy!"))
DEFINE_DIALOG(DIALOG_228, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_229, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_230, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_231, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_232, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_233, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_234, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_235, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_236, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_237, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_238, 1, 4, 30, 200, _("\
Yoshitext"))
DEFINE_DIALOG(DIALOG_239, 1, 4, 30, 200, _("\
Yoshitext"))