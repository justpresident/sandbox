#include <Adafruit_ssd1306syp.h>
#include <HardwareSerial.h>

#define SDA_PIN 8
#define SCL_PIN 9
Adafruit_ssd1306syp display(SDA_PIN,SCL_PIN);

#define MENU_MAX_ELEM 18


char*   menu_names[MENU_MAX_ELEM];
// 0 - parent menu
// 1 - integer
// 2 - time
// 3 - On/Off
uint8_t menu_type[MENU_MAX_ELEM];
int     menu_value[MENU_MAX_ELEM];
uint8_t menu_parent[MENU_MAX_ELEM];
uint8_t menu_child_first[MENU_MAX_ELEM];
uint8_t menu_child_end[MENU_MAX_ELEM];

uint8_t menu_rows_count = 4;
uint8_t menu_draw_from_pos = 0;
uint8_t menu_current_pos = 0;
uint8_t menu_editing = 0;

unsigned long long last_screen_update_time;

void menu_setup() {
	menu_current_pos = 4;
	menu_draw_from_pos = 4;
	
	menu_names[0]        = "Main Menu";
	menu_type[0]         = 0;
	menu_value[0]        = 0;
	menu_parent[0]       = 0;
	menu_child_first[0]  = 1;
	menu_child_end[0]    = 3;

	menu_names[1]        = "Menu1";
	menu_type[1]         = 0;
	menu_value[1]        = 0;
	menu_parent[1]       = 0;
	menu_child_first[1]  = 4;
	menu_child_end[1]    = 9;

	menu_names[2]        = "Menu2";
	menu_type[2]         = 0;
	menu_value[2]        = 0;
	menu_parent[2]       = 0;
	menu_child_first[2]  = 10;
	menu_child_end[2]    = 11;

	menu_names[3]        = "Menu3";
	menu_type[3]         = 0;
	menu_value[3]        = 0;
	menu_parent[3]       = 0;
	menu_child_first[3]  = 12;
	menu_child_end[3]    = 15;

	menu_names[4]        = "Param1_1";
	menu_type[4]         = 1;
	menu_value[4]        = 0;
	menu_parent[4]       = 1;
	menu_child_first[4]  = 0;
	menu_child_end[4]    = 0;

	menu_names[5]        = "Param1_2";
	menu_type[5]         = 2;
	menu_value[5]        = 0;
	menu_parent[5]       = 1;
	menu_child_first[5]  = 0;
	menu_child_end[5]    = 0;

	menu_names[6]        = "Param1_3";
	menu_type[6]         = 3;
	menu_value[6]        = 0;
	menu_parent[6]       = 1;
	menu_child_first[6]  = 0;
	menu_child_end[6]    = 0;

	menu_names[7]        = "Param1_4";
	menu_type[7]         = 1;
	menu_value[7]        = 0;
	menu_parent[7]       = 1;
	menu_child_first[7]  = 0;
	menu_child_end[7]    = 0;

	menu_names[8]        = "Param1_5";
	menu_type[8]         = 2;
	menu_value[8]        = 0;
	menu_parent[8]       = 1;
	menu_child_first[8]  = 0;
	menu_child_end[8]    = 0;

	menu_names[9]        = "Param1_6";
	menu_type[9]         = 3;
	menu_value[9]        = 0;
	menu_parent[9]       = 1;
	menu_child_first[9]  = 0;
	menu_child_end[9]    = 0;

	menu_names[10]       = "Param2_1";
	menu_type[10]        = 3;
	menu_value[10]       = 0;
	menu_parent[10]      = 2;
	menu_child_first[10] = 0;
	menu_child_end[10]   = 0;

	menu_names[11]       = "Param2_2";
	menu_type[11]        = 3;
	menu_value[11]       = 0;
	menu_parent[11]      = 2;
	menu_child_first[11] = 0;
	menu_child_end[11]   = 0;

	menu_names[12]       = "Param3_1";
	menu_type[12]        = 1;
	menu_value[12]       = 0;
	menu_parent[12]      = 3;
	menu_child_first[12] = 0;
	menu_child_end[12]   = 0;

	menu_names[13]       = "Param3_2";
	menu_type[13]        = 3;
	menu_value[13]       = 0;
	menu_parent[13]      = 3;
	menu_child_first[13] = 0;
	menu_child_end[13]   = 0;

	menu_names[14]       = "Param3_3";
	menu_type[14]        = 3;
	menu_value[14]       = 0;
	menu_parent[14]      = 3;
	menu_child_first[14] = 0;
	menu_child_end[14]   = 0;

	menu_names[15]       = "Menu3_4";
	menu_type[15]        = 0;
	menu_value[15]       = 0;
	menu_parent[15]      = 3;
	menu_child_first[15] = 16;
	menu_child_end[15]   = 17;

	menu_names[16]       = "Param3_4_1";
	menu_type[16]        = 2;
	menu_value[16]       = 0;
	menu_parent[16]      = 15;
	menu_child_first[16] = 0;
	menu_child_end[16]   = 0;

	menu_names[17]       = "Param3_4_2";
	menu_type[17]        = 3;
	menu_value[17]       = 0;
	menu_parent[17]      = 15;
	menu_child_first[17] = 0;
	menu_child_end[17]   = 0;
	
}

void setup() {
	Serial.begin(9600);
	menu_setup();
	display.initialize();
	last_screen_update_time = millis();
}

void loop() {
	draw_menu();

	process_buttons();
}

void process_buttons() {
	if (Serial.available() > 0) {
		int c = Serial.read();
		Serial.print("got: ");
		Serial.println(c);

		if (menu_editing > 0) {
			menu_edit(c);
		} else {
			menu_navigate(c);
		}
	}
}

void menu_edit(int c) {
	switch(c) {
		case 'h':
			//left
			menu_editing = 0;
			break;
		case 'k':
			//up
			menu_value[menu_current_pos]++;
			break;
		case 'j':
			//down
			menu_value[menu_current_pos]--; 
			break;
	}
	if (menu_editing) {
		switch (menu_type[menu_current_pos]) {
			case 2:
				// time
				if (menu_value[menu_current_pos] < 0)
					menu_value[menu_current_pos] = 0;
				break;
			case 3:
				// On/Off
				menu_value[menu_current_pos] = menu_value[menu_current_pos] > 0 ? 1 : 0;
				break;
		}
	}
}

void menu_navigate(int c) {
	switch(c) {
		case 'h':
			//left
			if (menu_parent[menu_current_pos] > 0) {
				menu_current_pos = menu_parent[menu_current_pos];
				menu_draw_from_pos = menu_child_first[menu_parent[menu_current_pos]];
			}
			break; 
		case 'l':
			//right
			if (menu_type[menu_current_pos] == 0) {
				menu_current_pos = menu_child_first[menu_current_pos];
				menu_draw_from_pos = menu_current_pos;
			} else {
				menu_editing = 1;
			}
			break; 
		case 'k':
			//up
			if (menu_current_pos > menu_child_first[menu_parent[menu_current_pos]]) {
				menu_current_pos--;
				if (menu_draw_from_pos > menu_current_pos)
					menu_draw_from_pos--;
			}
			break; 
		case 'j':
			//down
			if (menu_current_pos < menu_child_end[menu_parent[menu_current_pos]]) {
				menu_current_pos++;
				if (menu_draw_from_pos + menu_rows_count - 1 < menu_current_pos) {
					menu_draw_from_pos++;
				}
			}
			break; 
	}
}

void draw_menu() {
	if (millis() - last_screen_update_time < 100) {
		delay(millis() - last_screen_update_time);
	}
	last_screen_update_time = millis();

	uint8_t x_pos = 0, y_pos = 0;

	display.clear();
  	display.setTextSize(1);
  	display.setTextColor(WHITE);
	
	display.setCursor(x_pos, y_pos);
	display.print(menu_names[menu_parent[menu_current_pos]]);
	y_pos = 8;
	display.drawLine(x_pos, y_pos, SSD1306_WIDTH, y_pos, WHITE);

	for (uint8_t draw_pos = menu_draw_from_pos; draw_pos-menu_draw_from_pos < menu_rows_count; ++draw_pos) {
		if (draw_pos > menu_child_end[menu_parent[menu_current_pos]]) {
			break;
		}
		// item
		x_pos = 5;
		y_pos = 15 + 10 * (draw_pos - menu_draw_from_pos);
		display.setCursor(x_pos, y_pos);
		display.print(menu_names[draw_pos]);

		// value
		x_pos = 80;
		display.setCursor(x_pos, y_pos);
		int v = menu_value[draw_pos]; 
		switch(menu_type[draw_pos]) {
			case 0: // menu item
				display.print(">");
				break;
			case 1: // int
				display.print(v);
				break;
			case 2: // time
				display.print(v / 3600);
				display.print(":");
				display.print((v % 3600) / 60);
				display.print(":");
				display.print((v % 3600) % 60);
				break;
			case 3: // boolean
				display.print(v ? "on" : "off");
				break;
		}

		if (draw_pos == menu_current_pos) {
			uint8_t box_width, box_height = 10;
			y_pos -= 1;
			if (menu_editing == 0) {
				// rectangle around item
				x_pos = 3;
				box_width = 67;
			} else {
				// rectangle around value
				x_pos = 77;
				box_width = 45;
			}
			display.drawLine(x_pos,             y_pos,              x_pos + box_width, y_pos,              WHITE);
			display.drawLine(x_pos,             y_pos,              x_pos,             y_pos + box_height, WHITE);
			display.drawLine(x_pos,             y_pos + box_height, x_pos + box_width, y_pos + box_height, WHITE);
			display.drawLine(x_pos + box_width, y_pos,              x_pos + box_width, y_pos + box_height, WHITE);
		}
	}
	if (menu_draw_from_pos + menu_rows_count - 1 < menu_child_end[menu_parent[menu_current_pos]]) {
		display.setCursor(5, y_pos + 10);
		display.print("\\/");
	}

	display.update();
}


