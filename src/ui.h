#ifndef UI_H_
#define UI_H_

#ifdef __cplusplus
extern "C" {
#endif

int ui_init(void);
void ui_shutdown(void);

void ui_reshape(int x, int y);
void ui_draw(void);

void ui_button(int bn, int press, int x, int y);
void ui_motion(int x, int y);

#ifdef __cplusplus
}
#endif

#endif	/* UI_H_ */
