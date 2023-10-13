## Low level gang, dangling pointers and how I love to punish myself

There's always been something that tickled my fancy about low-level programming. Some call it systems programming, others low-level, and the last group would refer to it all as embedded (although that, again, can mean a stricter set of hardware - and an entirely different architectural paradim, but I digress).

It fascinates me how (pc) games can let the user tune the settings - here being graphics fedelity, physics, number of objects on-screen and more - to get the exact match that run best on their desired hardware, letting gamers and enthusiasts stress their systems as much as they want to achieve top performance.

And often, the telltale of optimized, well performant software in these cases, is how well you can run the software to run on your specific hardware. Can't polish a turd and all that. 

It's sad as I feel this is a far cry from the web-dev world where I dwell. We've gotten so framework and library-dependant. Many programmers I know will often reach for a library or extension to achieve the same things they could if they sat down and focused on writing code for an afternoon. 
With some care, it can be smaller and faster than the equivalent npm package but, obviously it depends. And this is coming from someone who eats and sleep express.js, asp/ .net/ core, and one more I can't remember.
Thing is though, shit's. sometimes. so. slow.

Programming on the esp8266 and the pico has been, to an extent, the most fun I've had actually writing something fairly complex and trying to get it to work. I love c, so developing the pico code in particular has me feeling like the Fresh Prince of Dev-Embedded. It's such a pallet cleanser, to write c and self-host as much as possible, and occassionaly slap myself for ever starting the project. 

Small, stupid fast, tiny footprint, and is about as safe as an wild elephant. I would probably never use it in a production environment outside of personal projects. c is really like portable assembly. Of course you can cast types, what does actual computer care? No types, pop the bytes*. 

If I were to put it another way. In the programming world, there's a spectrum of languages that take us from the ethereal realms of high-level cargo add abstraction to the nitty-gritty confines of low-level ~~masochism~~ ~~mastery~~ fun. Some are like soaring birds, while others, slide like a speedy slug. 

And right there, in the slug-juice, you'll find me and my beloved C, riding with the low-level gang, dancing with dangling pointers, hoping the compiler doesn't kick me out like Jazzy Jeff, and getting distracted by whatever you had to go research because you realised the problem wasn't a lack of knowledge of the framework or library you were using, but infact that you didn't understand the problem-domain enough. A computer processing and logical-thingy linux problem-domain**.

*N.B I can't assembly but I'm working on it ;) I'll C myself in-line out. 

**N.B note to future-self: finally figuring the problem out and effeciently solving will probably take too long. 