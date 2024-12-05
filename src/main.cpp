#include <Arduino.h>
#include <esp32_smartdisplay.h>
#include <Wire.h>
#include <vector>
// put function declarations here:

#define I2C_DEV_ADDR 0x55
//void InterfaceSys();
//TaskHandle_t Draw;
TaskHandle_t Inter;
byte incByte;
//lv_obj_t * extObj;
lv_obj_t * meter2;
lv_obj_t * shift;
lv_obj_t * batt;
lv_obj_t * canvas;

//Small text parts
lv_obj_t * tick1;
lv_obj_t * tick2;
lv_obj_t * tick3;
lv_obj_t * tick4;
lv_obj_t * tick5;
lv_obj_t * tick6;
lv_obj_t * tick7;
lv_obj_t * tick8;
lv_obj_t * tickP;
lv_obj_t * tickR;

uint32_t baseNum = 0;
uint32_t i = 0;
lv_area_t nextRect;
//lv_area_t nextRect;
bool inRead = 0;
bool gap = 0;
short alloc = 0;
//int baseNum;
char setGear = 'P';
char* setGearP = &setGear;

//Globalized crossframe parameters
//float pitch = 0;
//float roll = 0;
int bVal = 0;
float sVal = 0;

int recx1 = 45;
int recx2 = 55;
int recy1 = 45;
int recy2 = 55;



#define TILT_WIDTH  100
#define TILT_HEIGHT  100
lv_layer_t layer;
lv_draw_rect_dsc_t dsc;

void runMeter(void){
  static lv_style_t style;
  lv_style_init(&style);

  lv_style_set_arc_color(&style, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
  lv_style_set_arc_width(&style, 4);
  
    /*Create an object with the new style*/
  meter2 = lv_arc_create(lv_screen_active());
  //lv_obj_align(meter2, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  lv_obj_add_style(meter2, &style, 0);
  lv_obj_center(meter2);
  lv_arc_set_value(meter2, 0);
}
void loadTilt(){
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, TILT_WIDTH, TILT_HEIGHT,LV_COLOR_FORMAT_RGB565);
    canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    lv_canvas_fill_bg(canvas, lv_color_hex3(0x333), LV_OPA_COVER);
    lv_obj_center(canvas);

    
    lv_canvas_init_layer(canvas, &layer);

    
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = lv_palette_main(LV_PALETTE_RED);
    /*
    dsc.border_color = lv_palette_main(LV_PALETTE_BLUE);
    dsc.border_width = 3;
    dsc.outline_color = lv_palette_main(LV_PALETTE_GREEN);
    dsc.outline_width = 2;
    dsc.outline_pad = 2;
    dsc.outline_opa = LV_OPA_50;
    dsc.radius = 5;
    dsc.border_width = 3;
    */
    lv_area_t coords = {10, 10, 40, 30};

    lv_draw_rect(&layer, &dsc, &coords);

    lv_canvas_finish_layer(canvas, &layer);

    

}
void loadSet(){
  shift = lv_label_create(lv_screen_active());
  lv_obj_set_style_text_font(shift, &lv_font_montserrat_28, 0);
  lv_label_set_text(shift, "P");
  lv_obj_set_style_text_color(shift, lv_color_hex(0x80ff80), LV_PART_MAIN);
  lv_obj_align(shift, LV_ALIGN_CENTER, 140, -100);
}


void loadBack(void){
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x20202a), LV_PART_MAIN);

  lv_obj_t * label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Beta 1.0.1");
  lv_obj_set_style_text_color(label, lv_color_hex(0xff80ff), LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, -80);
}

void loadBatt(){
  batt = lv_label_create(lv_screen_active());
  lv_obj_set_style_text_font(batt, &lv_font_montserrat_28, 0);
  //lv_obj_set_style_text_align(batt, LV_TEXT_ALIGN_RIGHT,0);
  lv_label_set_text(batt, "10%");
  lv_obj_set_style_text_color(batt, lv_color_hex(0x80ff80), LV_PART_MAIN);
  //lv_obj_set_style_text_align(batt, LV_TEXT_ALIGN_RIGHT, LV_);
  lv_obj_align(batt, LV_ALIGN_RIGHT_MID, -245, -100);
}

void loadTextUnit(lv_obj_t * inst, int16_t posx, int16_t posy, const char * z, bool col){
  inst = lv_label_create(lv_screen_active());
  lv_label_set_text(inst, z);
  if (col){
    lv_obj_set_style_text_color(inst, lv_color_hex(0xf08010), LV_PART_MAIN);
  } else {
    lv_obj_set_style_text_color(inst, lv_color_hex(0x80ff80), LV_PART_MAIN);
  }
  lv_obj_align(inst,LV_ALIGN_CENTER, posx, posy);
}


float datFlush(std::vector<byte> numIn){
  byte numCount = numIn.size();
  char set = numIn[1];
  bool isDub = 0;
  bool isNeg = 0;
  double outF = 0;
  //int outH = 0;
  double expC = 0;
  if(set == 'G'){
    setGear = numIn[2];
    return 0;
  }
  if(numIn[2] == '-'){
    isNeg = 1;
  }
  if (numIn[0] != 20){//notice that numIn is inflated by 1, this simplifies the incriment checker
    isDub = 1;
    if(isNeg){
      expC = numCount - numIn[0] - 3;
    } else {
      expC = numCount - numIn[0] - 2;//needless ram use? Sure, but think of the casting savings!
    }
    
  } else {
    if(isNeg){
      expC = numCount - 4;
    } else {
      expC = numCount - 3;
    }
  }
  if (isNeg){
    for (byte i = 3; i < numCount; i++){
    outF = outF + (static_cast<double>(numIn[i]) - 48) * pow(10,expC);
    expC--;
  }
  outF = outF * -1;
  } else {
    for (byte i = 2; i < numCount; i++){
    outF = outF + (static_cast<double>(numIn[i]) - 48) * pow(10,expC);
    expC--;
  }
  }
  

  if (isDub){
    switch(set){
      case 'X':
      case 'Y':
      case 'Z':
        return static_cast<float>(outF);
        break;
      default:
        Serial.print("Bad char input A! What?"); 
    }
  } else {
    switch(set){
      case 'B':
        bVal = static_cast<int>(outF);
        break;
      case 'S':
        sVal = outF;
        break;
      default:
        Serial.print("Bad char input B! What?"); 
    }

  }
  return 0;
}

void regDupe(std::vector<byte> numIn, float & xVal, float & yVal, float & zVal){
  switch(numIn[1]){
        case 'X':
          xVal = datFlush(numIn);
        case 'Y':
          yVal = datFlush(numIn);
        case 'Z':
          zVal = datFlush(numIn);
        default:
          datFlush(numIn);
      }
}

void onRece(int len){
  /*
  while (Wire.available()) {
    Serial.write(Wire.read());
  }
  Serial.println();*/
  
  char nxt = 0;
  bool coordFlip = 0;
  bool tFlip = 0;
  byte counter = 0;
  float xVal = 0;
  float yVal = 0;
  float zVal = 0;
  std::vector<byte> tempNum;//store dmal place, char type, and num
  tempNum.push_back(20);
  while (Wire.available()) {
    nxt = Wire.read();
    if (nxt == ','){
      regDupe(tempNum, xVal, yVal, zVal);
      tempNum.clear();
      counter = 0;
      tFlip = 0;
      tempNum.push_back(20);//input for single var should never be more complex than 20
    } else {
      if (nxt != 'G' && !tFlip){
        if (nxt == '.'){
          tempNum[0] = counter;
        } else {
          if (nxt == 'X'){
            coordFlip = 1;
          }
          counter++;
          tempNum.push_back(nxt);
        }
      } else {
        tempNum.push_back(nxt);
        tFlip = 1;
      }
    }
  }
  regDupe(tempNum, xVal, yVal, zVal);

  if (coordFlip){
    float pitch = atan2(xVal, sqrt(yVal * yVal + zVal * zVal));//keep radians
    float roll = atan2(yVal, sqrt(xVal * xVal + zVal * zVal));
    if (roll < 0){
      recx2 = 55;
      recx1 = 45 + static_cast<int>(sin(roll) * 45.0);//max -45
    }else{
      recx1 = 45;
      recx2 = 55 + static_cast<int>(sin(roll) * 45.0);
    }
    if (pitch < 0){
      recy2 = 55;
      recy1 = 45 + static_cast<int>(sin(pitch) * 45.0);
    }else{
      recy1 = 55;
      recy2 = 55 + static_cast<int>(sin(pitch) * 45.0);
    }
  }
  //setGear = 'F';
  //Serial.println(pitch);
  //Serial.println(roll);
  //Serial.println(sVal);
}

void onRequ(){
  Wire.print(i++);
  Wire.print(" Packets.");
  Serial.println("onRequest");
  Serial.println();
} 

//Handle serial IO and logic
void InterfaceSys(void * pvParameters){
  Wire.onReceive(onRece);
  Wire.onRequest(onRequ);
  Wire.begin((uint8_t)I2C_DEV_ADDR);
  
  for(;;){
    vTaskDelay(33);
  }
}

uint32_t millis_cb(void) {
  return millis();
}

void setup()
{
  Serial.begin(9600);
  xTaskCreatePinnedToCore(
                    InterfaceSys,
                    "Inter",
                    10000,
                    NULL,
                    1,
                    &Inter,
                    0);
  delay(100); 

  //lv_init();
  
  smartdisplay_init();
  lv_tick_set_cb(millis_cb);
  auto display = lv_display_get_default();
  lv_display_set_rotation(display, LV_DISPLAY_ROTATION_90);
  // lv_display_set_rotation(display, LV_DISPLAY_ROTATION_180);
  // lv_display_set_rotation(display, LV_DISPLAY_ROTATION_270);
  loadBack();
  //runArc();
  //lv_obj_set_pos(extObj,-80,-15);
  runMeter();
  lv_obj_set_pos(meter2,-75,15);
  loadTilt();
  lv_obj_set_pos(canvas,90,10);
  loadSet();
  loadBatt();
  loadTextUnit(tick1,-110,50,"0",0);
  loadTextUnit(tick2,-125,22,"1",0);
  loadTextUnit(tick3,-120,-10,"2",0);
  loadTextUnit(tick4,-93,-32,"3",0);
  loadTextUnit(tick5,-58,-32,"4",0);
  loadTextUnit(tick6,-30,-10,"5",0);
  loadTextUnit(tick7,-25,22,"6",0);
  loadTextUnit(tick8,-40,50,"7",0);
  loadTextUnit(tickP,20,10,"Pitch",1);
  loadTextUnit(tickR,92,-42,"Roll",1);
  /*
  lv_area_t arrtst = {35, 10, 45, 70};
  lv_draw_rect(&layer, &dsc, &arrtst);
  lv_canvas_finish_layer(canvas, &layer);*/

  //lv_task_create(updateRegion, 1000, LV_TASK_PRIO_MID, NULL);
}


void loop() {
  // Update the ticker
  //if (adjByte != 0){
    //lv_arc_set_value(extObj, adjByte);
    lv_arc_set_value(meter2, (sVal*100.0) / 120.0);
    lv_label_set_text(shift, reinterpret_cast<const char*>(setGearP));
    lv_label_set_text(batt, (std::to_string(bVal) + "%").c_str());
    // Unstable! do not uncomment unless ready (batt, const_cast<const char*>(strcat(reinterpret_cast<char*>(bVal),"%")))
    lv_canvas_fill_bg(canvas, lv_color_hex3(0x333), LV_OPA_COVER);
    nextRect = {recx1, 10, recx2, 90};
    lv_draw_rect(&layer, &dsc, &nextRect);
    nextRect = {10, recy1, 90, recy2};
    lv_draw_rect(&layer, &dsc, &nextRect);
    lv_canvas_finish_layer(canvas, &layer);
    //adjByte = 0;
  //}
  lv_timer_handler();
  // put your main code here, to run repeatedly:
  
}