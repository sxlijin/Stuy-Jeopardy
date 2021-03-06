/**
* Stuyvesant Jeopardy System
* @author FRC694
*
* This system is a replica of Jeopardy and works as follows:
* - There are 4 "contestants": 1 parent, 1 teacher, 1 alumnus, 1 team of students
*   - The team of students consists of 4 students
* - The moderator asks a question.
* - The contestants buzz to answer the question
*   - The buzzer sounds to indicate that the answering period has begun
*   - The contestant has 5 seconds to answer the question
*     - If the constestant answers correctly, the moderator presses his button, after which any contestant may buzz to answer again.
*     - If the contestant answers falsely (or doesn't answer), everyone must wait until his questioning period is up, at which point the buzzer sounds twice and anyone else may answer.
* - If any student answers falsely, the entire team of students is seen as having answered falsely.
* - A contestant has 5 seconds to answer a question after buzzing and being selected to answer.
*/

unsigned int contest_pin[] = {A0,A1,A2,A3,A4,A5,2}; // input lines for buttons; the first 6 (0-5) are analog ins; the last one is a digital
unsigned int moderator_pin = 3; // input line for moderator button (digital input)

unsigned int contest_light[] = {6,7,8,9,10,11,12}; // output lines for lights
// unsigned int moderator_light = 3; // why is this here????

unsigned int claxon_pin = 4; // GPIO pin on the speaker
int i; // iterator

boolean correct_answer = false;
boolean hasAnswered[] = {false, false, false, false};

void setup() {
  Serial.begin(9600);
  
  for (i = 0; i < 7; i++)  {
    pinMode(contest_pin[i], INPUT); // allocate and configure contestant input lines
    pinMode(contest_light[i], OUTPUT); // allocate and configure contestant output lines
  }
  pinMode(moderator_pin, INPUT); // allocate and configure moderator input
  //pinMode(moderator_light, OUTPUT);
  pinMode(claxon_pin, OUTPUT); // allocate and configure speaker line
  
  i = 0; // initialize iterator
}

void loop() {
  Serial.print("loop conditions: ");
  Serial.print(isPressed(i)); Serial.print(", ");
  Serial.println( (getAnsweredState(i) == false) );
  if (isPressed(i) && (getAnsweredState(i) == false)) {
    Serial.println("Someone who's still allowed to answer has buzzed in!")
    Serial.print("Index (i) is: ");
    Serial.println(i);
    Serial.print("isPressed returns the following for i: ");
    Serial.println(isPressed(i));
    Serial.print("not getAnsweredState (i.e. are they allowed to answer?) says: ");
    Serial.println(!getAnsweredState(i));
    // if contestant who *can* answer questions...
    long int start = millis();
    
    lightSet(i, HIGH); //Enable the corresponding light
    signalStart(claxon_pin); //Sound the claxon!
    
    correct_answer = false; //Contestant can only get it right if mod says he got it right!
    
    while (millis() - start < 5000) { // 5 seconds for contestant to answer AND 
      if (isPressed(moderator_pin)) { // for moderator to confirm the answer
        correct_answer = true;
        break;
      }
    }
    
    signalEnd(claxon_pin); // signal that the waiting period is over or question has been answered
    lightSet(i, LOW); //light off
    
    if (correct_answer)
      resetAnswerStates(); // The question's over - everyone can answer again for the next question
    else
      setAnswerState(i, true); // Prevent contestant [group] from answering this question again.
  }
  Serial.print("mod pin is ");
  Serial.println(isPressed(moderator_pin));
  if (isPressed(moderator_pin) && (hasAnswered[] == {true, true, true, true})) {
    resetAnswerStates(); // If everyone gets locked out, mod presses his button to reset the system.
  }
  
  i = (i + 1) % 7; //Cycles through all 7 contestants.
}

boolean getAnsweredState(int contestant) {
  if (contestant > 2)
    return hasAnswered[3]; //Students altogether only get one chance at answering
  else
    return hasAnswered[contestant]; //Everyone else has an individual chance
}

void setAnswerState(int contestant, boolean set) {
  if (contestant > 2)
    hasAnswered[3] = set; //Students altogether only get one chance at answering
  else
    hasAnswered[contestant] = set; //Everyone else has an individual chance
}

boolean isPressed(unsigned int i) {
  // All the switches are pulled up
  if (digitalRead(contest_pin[i])) //-- meaning that it is HIGH by 
    return false; // So in its default state (unpressed), digitalRead returns HIGH, i.e. TRUE
  else 
    return true; //whereas in its active state (pressed), digitalRead returns LOW, i.e. FALSE
}

void resetAnswerStates() {
  hasAnswered[] = {false, false, false, false};
}

void lightSet(unsigned int i, boolean state) {
  digitalWrite(contest_light[7 - i - 1], state);
}

void signalStart(unsigned int speaker_line) {
  int frequency = 150;
  int duration = 200;


  tone(speaker_line, frequency);
  delay(duration);
  noTone(speaker_line);
}

void signalEnd(unsigned int speaker_line) {
  //pulses the speaker twice in quick succession
  
  int frequency = 300; // in Hz
  int duration = 75; //in msec
  
  tone(speaker_line, frequency);
  delay(duration);
  noTone(speaker_line);
  delay(50);
  tone(speaker_line, frequency);
  delay(duration);
  noTone(speaker_line);
}
