#ifndef __NXCTRL_TYPES_HEADER__
#define __NXCTRL_TYPES_HEADER__

typedef unsigned int                    NXCTRL_UINT32;
typedef unsigned short                  NXCTRL_UINT16;
typedef void                            NXCTRL_VOID;
typedef int                             NXCTRL_INT32;
typedef unsigned char                   NXCTRL_UINT8;

typedef char                            NXCTRL_BOOL;
#define NXCTRL_TRUE                     ((NXCTRL_BOOL)1)
#define NXCTRL_FALSE                    ((NXCTRL_BOOL)0)
#define NXCTRL_ON                       NXCTRL_TRUE
#define NXCTRL_OFF                      NXCTRL_FALSE
#define NXCTRL_HIGH                     NXCTRL_TRUE
#define NXCTRL_LOW                      NXCTRL_FALSE

typedef char                            NXCTRL_DIR;
#define NXCTRL_INPUT                    ((NXCTRL_DIR)0)
#define NXCTRL_OUTPUT                   ((NXCTRL_DIR)1)
#define NXCTRL_INPUT_PULLUP             ((NXCTRL_DIR)2)
#define NXCTRL_OUTPUT_PULLUP            ((NXCTRL_DIR)3)
#define NXCTRL_INPUT_PULLDN             ((NXCTRL_DIR)4)
#define NXCTRL_OUTPUT_PULLDN            ((NXCTRL_DIR)5)

typedef char                            NXCTRL_PWM_AQ_MODE;
#define PWM_AQ_NONE                     ((NXCTRL_PWM_AQ_MODE)0x0)
#define PWM_AQ_LOW                      ((NXCTRL_PWM_AQ_MODE)0x1)
#define PWM_AQ_HIGH                     ((NXCTRL_PWM_AQ_MODE)0x2)
#define PWM_AQ_TOGGLE                   ((NXCTRL_PWM_AQ_MODE)0x3)
typedef int                             NXCTRL_BANK;
#define NXCTRL_P8                       ((NXCTRL_BANK)0)
#define NXCTRL_P9                       ((NXCTRL_BANK)1)

typedef int                             NXCTRL_PIN;
#define NXCTRL_PIN01                    ((NXCTRL_PIN)1)
#define NXCTRL_PIN02                    ((NXCTRL_PIN)2)
#define NXCTRL_PIN03                    ((NXCTRL_PIN)3)
#define NXCTRL_PIN04                    ((NXCTRL_PIN)4)
#define NXCTRL_PIN05                    ((NXCTRL_PIN)5)
#define NXCTRL_PIN06                    ((NXCTRL_PIN)6)
#define NXCTRL_PIN07                    ((NXCTRL_PIN)7)
#define NXCTRL_PIN08                    ((NXCTRL_PIN)8)
#define NXCTRL_PIN09                    ((NXCTRL_PIN)9)
#define NXCTRL_PIN10                    ((NXCTRL_PIN)10)
#define NXCTRL_PIN11                    ((NXCTRL_PIN)11)
#define NXCTRL_PIN12                    ((NXCTRL_PIN)12)
#define NXCTRL_PIN13                    ((NXCTRL_PIN)13)
#define NXCTRL_PIN14                    ((NXCTRL_PIN)14)
#define NXCTRL_PIN15                    ((NXCTRL_PIN)15)
#define NXCTRL_PIN16                    ((NXCTRL_PIN)16)
#define NXCTRL_PIN17                    ((NXCTRL_PIN)17)
#define NXCTRL_PIN18                    ((NXCTRL_PIN)18)
#define NXCTRL_PIN19                    ((NXCTRL_PIN)19)
#define NXCTRL_PIN20                    ((NXCTRL_PIN)20)
#define NXCTRL_PIN21                    ((NXCTRL_PIN)21)
#define NXCTRL_PIN22                    ((NXCTRL_PIN)22)
#define NXCTRL_PIN23                    ((NXCTRL_PIN)23)
#define NXCTRL_PIN24                    ((NXCTRL_PIN)24)
#define NXCTRL_PIN25                    ((NXCTRL_PIN)25)
#define NXCTRL_PIN26                    ((NXCTRL_PIN)26)
#define NXCTRL_PIN27                    ((NXCTRL_PIN)27)
#define NXCTRL_PIN28                    ((NXCTRL_PIN)28)
#define NXCTRL_PIN29                    ((NXCTRL_PIN)29)
#define NXCTRL_PIN30                    ((NXCTRL_PIN)30)
#define NXCTRL_PIN31                    ((NXCTRL_PIN)31)
#define NXCTRL_PIN32                    ((NXCTRL_PIN)32)
#define NXCTRL_PIN33                    ((NXCTRL_PIN)33)
#define NXCTRL_PIN34                    ((NXCTRL_PIN)34)
#define NXCTRL_PIN35                    ((NXCTRL_PIN)35)
#define NXCTRL_PIN36                    ((NXCTRL_PIN)36)
#define NXCTRL_PIN37                    ((NXCTRL_PIN)37)
#define NXCTRL_PIN38                    ((NXCTRL_PIN)38)
#define NXCTRL_PIN39                    ((NXCTRL_PIN)39)
#define NXCTRL_PIN40                    ((NXCTRL_PIN)40)
#define NXCTRL_PIN41                    ((NXCTRL_PIN)41)
#define NXCTRL_PIN42                    ((NXCTRL_PIN)42)
#define NXCTRL_PIN43                    ((NXCTRL_PIN)43)
#define NXCTRL_PIN44                    ((NXCTRL_PIN)44)
#define NXCTRL_PIN45                    ((NXCTRL_PIN)45)
#define NXCTRL_PIN46                    ((NXCTRL_PIN)46)

typedef char                            NXCTRL_AIN;
#define NXCTRL_A0                       ((NXCTRL_AIN)0)
#define NXCTRL_A1                       ((NXCTRL_AIN)1)
#define NXCTRL_A2                       ((NXCTRL_AIN)2)
#define NXCTRL_A3                       ((NXCTRL_AIN)3)
#define NXCTRL_A4                       ((NXCTRL_AIN)4)
#define NXCTRL_A5                       ((NXCTRL_AIN)5)
#define NXCTRL_A6                       ((NXCTRL_AIN)6)

typedef int                             NXCTRL_MODE;
#define NXCTRL_MODE0                    ((NXCTRL_MODE)0)
#define NXCTRL_MODE1                    ((NXCTRL_MODE)1)
#define NXCTRL_MODE2                    ((NXCTRL_MODE)2)
#define NXCTRL_MODE3                    ((NXCTRL_MODE)3)
#define NXCTRL_MODE4                    ((NXCTRL_MODE)4)
#define NXCTRL_MODE5                    ((NXCTRL_MODE)5)
#define NXCTRL_MODE6                    ((NXCTRL_MODE)6)
#define NXCTRL_MODE7                    ((NXCTRL_MODE)7)

typedef int                             NXCTRL_PULLTYPE;
#define NXCTRL_NOPULL                   ((NXCTRL_PULLTYPE)0)
#define NXCTRL_PULLUP                   ((NXCTRL_PULLTYPE)1)
#define NXCTRL_PULLDN                   ((NXCTRL_PULLTYPE)2)

typedef void                            (*NXCTRL_SIGINT_HANDLER) (void);

#endif /* __NXCTRL_TYPES_HEADER__ */
