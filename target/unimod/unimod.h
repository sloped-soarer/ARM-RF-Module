// Function Prototypes

void CLOCK_delayms(u32 delay_ms);
void uni_mod(void);
void usart_three_setup(void); // Debug Port.
const void * select_protocol_cmds(enum Protocols);
static int get_module(enum Protocols);
void set_proto_leds(enum Protocols);
void timer_enable_1us_oc1(u32 timer_peripheral);
u8 read_bind_sw(void);
void change_protocol(void); // put in header
void initialise_hardware(void);
void display_proto_options(enum Protocols idx);
s8 bind_button_duration(void);


// Port bit #defines for uni module.

// PORT A
#define BIND_SWITCH GPIO1
// #define USART2_TX GPIO2 // Already defined in Libopencm3. Also use GPIO_BANK_USART2_TX when using gpio_set_mode().
// #define USART2_RX GPIO3
#define LED_G GPIO4
#define LED_R GPIO5
#define LED_Y GPIO6
#define LED_O GPIO7
#define SPARE3 GPIO8
// #define TIM1_CH2 GPIO9
// #define USART1_TX GPIO9
// #define USART1_RX GPIO10
#define SPARE2 GPIO11
#define SPARE1 GPIO12
#define SWDIO GPIO13
#define SWCLK GPIO14 

// PORT B
#define ALT_CONF GPIO0
#define CH_ORD GPIO1
#define BOOT1 GPIO2
#define SWO GPIO3
#define SCIO GPIO5
#define HEARTBEAT_CPU GPIO6
#define RF_EN GPIO7
#define S_PORT_EN GPIO9
// #define USART3_TX GPIO10
// #define USART3_RX GPIO11
#define CS GPIO12
//#define SCK GPIO13
//#define MISO GPIO14
//#define MOSI GPIO15

#define LED_TOGGLE(colour) gpio_toggle(GPIOA, colour)
#define LED_ON(colour)	gpio_set(GPIOA, colour)
#define LED_OFF(colour)	gpio_clear(GPIOA, colour)
#define RF_EN_STATE()	gpio_get(GPIOB, RF_EN)

//enum PROTO_MODE {
//BIND_MODE,
//NORMAL_MODE,
//RANGE_MODE,
//};


