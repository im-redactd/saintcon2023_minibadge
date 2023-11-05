

class Friends {
  public:
    Friends(){};
    bool runPattern();
  private:
	int buttonMode = 0;
};


bool Friends::runPattern() {
	
	checkForTouch();
	
	//uint16_t cap = get_raw_reading();
	//tickForPersistenceEvery30Sec();
	
	#ifdef WE_HAVE_A_FRIEND
		
		// static set delay for text
		int16_t frame_delay = 50;

		uint8_t size = max(int(kMatrixWidth/12), 1);
		matrix->setTextSize(size);
		matrix->setTextColor(FRIEND_COLOR);

		//matrix_clear();
		matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
		matrix->setTextSize(1);
		matrix->setRotation(0);
		int name_length = strlen(FRIEND_NAME);
		for (int16_t x=8*size; x>=-6*name_length*size; x--) {
			matrix_clear();
			if(FRIEND_SPARKLE) Sparkle3(random(255), random(255), random(255), 25);
			matrix->setCursor(x,kMatrixWidth/2-size*4);
			matrix->print(FRIEND_NAME);
			matrix_show();
		// loop the frame delay so we can listen for buttons.
		for (size_t tick = 0; tick < frame_delay; tick++)
		{
			checkForTouch();
			
			if (modeInterruptRequested) {
				rtt.println("friends modeinterrupt");
				modeInterruptRequested = false;
				return false;
			}
			if (buttonInterruptRequested) {
				rtt.println("friends buttoninterrupt");
				modePushCounter = 0;
				buttonInterruptRequested = false;
				return false;
			}
			
			delay(1);
		}
	}
	
    #endif

	return true;
}
