#include <Arduino.h>
#include <esp32_smartdisplay.h>
//#include <Wire.h>
// put function declarations here:

//#define I2C_DEV_ADDR 0x55
//void InterfaceSys();
//TaskHandle_t Draw;
TaskHandle_t Inter;
byte incByte;
lv_obj_t * extObj;
lv_obj_t * meter2;
lv_obj_t * shift;
uint32_t adjByte = 0;
uint32_t baseNum = 0;
bool inRead = 0;
bool gap = 0;
short alloc = 0;
//int baseNum;
char setGear = 'P';
char* setGearP = &setGear;

/* 
#define TILT_WIDTH  50
#define TILT_HEIGHT  50
*/
void runArc(void){
  static lv_style_t style;
  lv_style_init(&style);

  lv_style_set_arc_color(&style, lv_palette_main(LV_PALETTE_RED));
  lv_style_set_arc_width(&style, 4);

    /*Create an object with the new style*/
  extObj = lv_arc_create(lv_screen_active());
  lv_obj_add_style(extObj, &style, 0);
  lv_obj_center(extObj);
  lv_arc_set_value(extObj, 40);
}
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
/* temp
void loadTilt(){
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, CANVAS_WIDTH, CANVAS_HEIGHT,LV_COLOR_FORMAT_RGB565);
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER);
    lv_obj_center(canvas);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = lv_palette_main(LV_PALETTE_RED);
    dsc.border_color = lv_palette_main(LV_PALETTE_BLUE);
    dsc.border_width = 3;
    dsc.outline_color = lv_palette_main(LV_PALETTE_GREEN);
    dsc.outline_width = 2;
    dsc.outline_pad = 2;
    dsc.outline_opa = LV_OPA_50;
    dsc.radius = 5;
    dsc.border_width = 3;

    lv_area_t coords = {10, 10, 40, 30};

    lv_draw_rect(&layer, &dsc, &coords);

    lv_canvas_finish_layer(canvas, &layer);
}
*/
void loadSet(){
  shift = lv_label_create(lv_screen_active());
  lv_label_set_text(shift, "P");
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0x80ff80), LV_PART_MAIN);
  lv_obj_align(shift, LV_ALIGN_TOP_RIGHT, 0, 0);
}


void loadBack(void){
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x20202a), LV_PART_MAIN);

  lv_obj_t * label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Speed Boy");
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}
/*
void onRece(int len){

}

void onRequ(){

} */

//Handle serial IO and logic
void InterfaceSys(void * pvParameters){
  //Wire.onReceive(onRece);
 //Wire.onRequest(onRequ);
  for(;;){
    if (Serial.available() > 0) {
      //Serial.print("Cereal running on core ");
      //Serial.println(xPortGetCoreID());
      incByte = Serial.read();
    // say what you got:
      //Serial.print("Eating a burger with no: ");
      //Serial.println(incByte, DEC);
      adjByte = incByte / 2;
      if(inRead){
        if(incByte == 125){//Cancels read when "}" is found
          inRead = false;
        } else {
          if(gap){ //diet hash table
            if (incByte == 115){//s
              alloc = 1;
            }
            else if (incByte == 103){//g
              alloc = 2;
            }
            gap = false;
          } else {
            if (incByte == 32){
              gap = true;
            } else {
              if (alloc == 1){
                baseNum = incByte - 48;
              }
              else if (alloc == 2){
                setGear = incByte;
              }
            }

          }
        }
      }
      if(incByte == 123 && !inRead){//Starts read when "{" is found outside of read
        inRead = true;
        gap = true;
      }

    }
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
  runArc();
  lv_obj_set_pos(extObj,-80,-15);
  runMeter();
  lv_obj_set_pos(meter2,80,-15);
  //loadTilt();
  loadSet();

  //lv_task_create(updateRegion, 1000, LV_TASK_PRIO_MID, NULL);
}


void loop() {
  // Update the ticker
  //if (adjByte != 0){
    lv_arc_set_value(extObj, adjByte);
    lv_arc_set_value(meter2, baseNum * 10);
    lv_label_set_text(shift, reinterpret_cast<const char*>(setGearP));
    //adjByte = 0;
  //}
  lv_timer_handler();
  // put your main code here, to run repeatedly:
  
}

// put function definitions here:

