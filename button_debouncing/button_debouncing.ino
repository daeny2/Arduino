#define PS 7 //11
#define MAX_BTN_CNT  1

int buttonCurrState[MAX_BTN_CNT];
int buttonSaveState[MAX_BTN_CNT];
int buttonAction[MAX_BTN_CNT];
long pressTime[MAX_BTN_CNT];

int buttonPresses = 0;

boolean btnToggle = false;

void setup() {
  Serial.begin(9600);
  pinMode(PS, INPUT);
  pinMode(13, OUTPUT);
  for(int i = 0; i < MAX_BTN_CNT; i++)
    buttonSaveState[i] = digitalRead(PS);
    
  Serial.println("starting");
}


int button_debounce_init(int btnNo) {
  buttonAction[btnNo] = 0;
}

int button_debounce(int btnNo, int btnPin) {
  buttonCurrState[btnNo] = digitalRead(btnPin);

  if(buttonCurrState[btnNo] != buttonSaveState[btnNo]) {
    pressTime[btnNo] = millis();
  }

  if(pressTime[btnNo] + 50 > millis()) {
    if(buttonCurrState[btnNo] != buttonSaveState[btnNo]) {
        buttonSaveState[btnNo] = buttonCurrState[btnNo];
        buttonAction[btnNo]++;
    }
  }
  
  return buttonAction[btnNo];
}

void loop() {
  if(2 == button_debounce(0, PS)) {
    button_debounce_init(0);
    buttonPresses++;
    Serial.print("Button pressed : ");
    Serial.println(buttonPresses, DEC);
    digitalWrite(13, (btnToggle) ? HIGH : LOW);
    btnToggle = ~btnToggle;
  }
}
