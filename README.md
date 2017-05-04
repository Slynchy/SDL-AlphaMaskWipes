# SDL-AlphaMaskWipes
An SDL2 API for drawing grayscale transitions/wipes.

# How to use
Include "Transition.h", and create a new Transition object as such:

    // params: renderer, filename, type (fadein or out), speed
    Transition* testTrans = new Transition(SDL_RENDERER, "wipe.png", TRANS_TYPE::IN, 6);
	
Then when the transition needs to occur, simply call the following in your main loop:

	testTrans->Update(SDL_RENDERER);
	SDL_RenderCopy(RENDERER, testTrans->texture, NULL, NULL);
	
You can check if the transition is complete with:

	testTrans->isComplete(); // returns true if complete