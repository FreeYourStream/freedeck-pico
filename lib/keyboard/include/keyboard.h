#ifdef __cplusplus
extern "C" {
#endif
void init_hid(void);
void process_hid(void);
void cdc_task(void);
void setKeycode(uint8_t newKeycode[6]);
#ifdef __cplusplus
}
#endif