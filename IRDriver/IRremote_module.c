#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kobject.h> 
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GESL");
MODULE_DESCRIPTION("A simple Linux GPIO IR remote");
MODULE_VERSION("0.1");

#define DATA_MASK 0xFFFFFF
int unique_val(void);
static bool last_t_bit = 0, current_t_bit;
static bool isPulse = 0;
static unsigned long data;
static struct input_dev *button_dev;

static unsigned int gpioIr =902+24; 
static unsigned long long rbuf=0;
static unsigned int  nbits = 0;

static char   gpioName[8] = "gpioXXX";
static int    irqNumber;
static struct timespec ts_last, ts_current, ts_diff;

static irq_handler_t  irgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

/*
   Function name: unique_val
Description : Return the key code by passing unique key value in switch case.
*/
int unique_val(void)
{
    current_t_bit = (data & 0xFF000000)? 0 : 1;
    if(current_t_bit != last_t_bit){}

    printk(KERN_INFO "inside fun data = %lx",data); 

    switch(data){
        case 0x100114:
            printk("Button pressed = \" Play \" \n");
            return KEY_PLAY;
            break;

        case 0x100141:
            printk("Button pressed = \" Stop \" \n");
            return KEY_STOP;
            break;

        case 0x100050:
            printk("Button pressed = \" Power \" \n");
            return KEY_POWER;
            break;

        case 0x100115:
            printk("Button pressed = \" Record \" \n");
            return KEY_RECORD;
            break;

        case 0x100404:
            printk("Button pressed = \" OK \" \n");
            return KEY_ENTER; 
            break;

        case 0x40100404:
            printk("Button pressed = \" OK \" \n");
            return KEY_ENTER;
            break;

        case 0x100405:
            printk("Button pressed = \" Back \" \n");
            return KEY_BACK; 
            break;

        case 0x100145:
            printk("Button pressed = \" Replay \" \n");
            return 16;
            break;

        case 0x100144:
            printk("Button pressed = \" Skip \" \n");
            return 17; 
            break;

        case 0x100111:
            printk("Button pressed = \" Rew \" \n");
            return KEY_REWIND;
            break;

        case 0x100401:
            printk("Button pressed = \" Right \" \n");
            return KEY_RIGHT;
            break;

        case 0x100400:
            printk("Button pressed = \" Left \" \n");
            return KEY_LEFT;
            break;

        case 0x40100401:
            printk("Button pressed = \" Right \" \n");
            return KEY_RIGHT;
            break;

        case 0x40100400:
            printk("Button pressed = \" Left \" \n");
            return KEY_LEFT;
            break;

        case 0x100154:
            printk("Button pressed = \" Up \" \n");
            return KEY_UP;
            break;

        case 0x100155:
            printk("Button pressed = \" Down \" \n");
            return KEY_DOWN;
            break;

        case 0x100100:
            printk("Button pressed = \" Vol+ \" \n");
            return KEY_VOLUMEUP;
            break;

        case 0x100101:
            printk("Button pressed = \" Vol- \" \n");
            return KEY_VOLUMEDOWN;
            break;

        case 0x100104:
            printk("Button pressed = \" Ch+ \" \n");
            return KEY_CHANNELUP;
            break;

        case 0x100105:
            printk("Button pressed = \" Ch- \" \n");
            return KEY_CHANNELDOWN;
            break;

        case 0x100051:
            printk("Button pressed = \" Start \" \n");
            return KEY_HOME;
            break;

        case 0x100054:
            printk("Button pressed = \" Mute \" \n");
            return KEY_MUTE;
            break;

        case 0x101040:
            printk("Button pressed = \" Record TV \" \n");
            return 27;
            break;

        case 0x100414:
            printk("Button pressed = \" Guide \" \n");
            return KEY_PROGRAM;
            break;

        case 0x100411:
            printk("Button pressed = \" Live TV \" \n");
            return 29;
            break;

        case 0x100410:
            printk("Button pressed = \" DVD Menu \" \n");
            return KEY_DVD;
            break;

        case 0x100110:
            printk("Button pressed = \" Fwd \" \n");
            return KEY_FORWARD;
            break;

        case 0x100140:
            printk("Button pressed = \" Pause \" \n");
            return KEY_PAUSE;
            break;

        case 0x100055:
            printk("Button pressed = \" More \" \n");
            return 33;
            break;

        default:
            break;
    }
    return 0;
}
/*
   Function name: irdata_show
Description : To display the key's unique value
*/
static ssize_t irdata_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%ld\n", data); 
}


static struct kobj_attribute ir_attr = __ATTR_RO(irdata);


static struct attribute *ir_attrs[] = {
    &ir_attr.attr,
    NULL,
};


static struct attribute_group attr_group = {
    .name  = gpioName,
    .attrs = ir_attrs,
};


static struct kobject *ir_kobj;

/*
   Function name: irRemote_init
Description : Creating a sysfs entry and registering IRremote has input deivce
*/
static int __init irRemote_init(void){
    int result = 0;
    int error;
    unsigned long IRQflags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;

    printk(KERN_INFO "IR Remote: Initializing the IR Remote LKM\n");
    sprintf(gpioName, "gpio%d", gpioIr);
    ir_kobj = kobject_create_and_add("irRemote", kernel_kobj->parent);     //Creating an sysfs entry
    if(!ir_kobj){
        printk(KERN_ALERT "IR Remote: failed to create kobject mapping\n");
        return -ENOMEM;
    }

    result = sysfs_create_group(ir_kobj, &attr_group);
    if(result) {
        printk(KERN_ALERT "IR Remote: failed to create sysfs group\n");
        kobject_put(ir_kobj);
        return result;
    }
    getnstimeofday(&ts_last);
    ts_diff = timespec_sub(ts_last, ts_last);


    gpio_request(gpioIr, "sysfs");          //requesting for gpio pin
    gpio_direction_input(gpioIr);           //setting gpio pin as input pin
    gpio_export(gpioIr, false);

    printk(KERN_INFO "IR Remote: The gpio state is currently: %d\n", gpio_get_value(gpioIr));

    irqNumber = gpio_to_irq(gpioIr);        //Requesting for irq number  
    printk(KERN_INFO "IR Remote: The irData is mapped to IRQ: %d\n", irqNumber);

    button_dev = input_allocate_device();     //Allocating memory  for input device struct
    if (!button_dev) {
        printk(KERN_ERR "button.c: Not enough memory\n");
    }
    button_dev->evbit[0] = BIT_MASK(EV_KEY);
    button_dev->name = "IRremote";

    set_bit(EV_KEY, button_dev->evbit);            
    set_bit(KEY_LEFT,button_dev->keybit);
    set_bit(KEY_RIGHT,button_dev->keybit);
    set_bit(KEY_UP,button_dev->keybit);
    set_bit(KEY_DOWN,button_dev->keybit);
    set_bit(KEY_ENTER,button_dev->keybit);
    set_bit(KEY_BACK,button_dev->keybit);
    set_bit(KEY_VOLUMEUP,button_dev->keybit);
    set_bit(KEY_VOLUMEDOWN,button_dev->keybit);
    set_bit(KEY_CHANNELUP,button_dev->keybit);
    set_bit(KEY_CHANNELDOWN,button_dev->keybit);
    set_bit(KEY_HOME,button_dev->keybit);
    set_bit(KEY_FORWARD,button_dev->keybit);
    set_bit(KEY_PAUSE,button_dev->keybit);
    set_bit(KEY_DVD,button_dev->keybit);
    set_bit(KEY_PROGRAM,button_dev->keybit);
    set_bit(KEY_REWIND,button_dev->keybit);
    set_bit(KEY_POWER,button_dev->keybit);
    set_bit(KEY_STOP,button_dev->keybit);
    set_bit(KEY_PLAY,button_dev->keybit);
    set_bit(KEY_PAUSE,button_dev->keybit);

    error = input_register_device(button_dev);              //Registering IR remote has input device
    if (error) {
        printk(KERN_ERR "button.c: Failed to register device\n");
    }

    result = request_irq(irqNumber,                          //Requesting for interrupt when button is pressed
            (irq_handler_t) irgpio_irq_handler,
            IRQflags,
            "ir_remote_handler",
            NULL);
    return result;
}

/*
   Function name: irRemote_exit 
Description : Deallocate all the memory and free irq
*/

static void __exit irRemote_exit(void){
    kobject_put(ir_kobj);
    free_irq(irqNumber, NULL);
    gpio_unexport(gpioIr);
    gpio_free(gpioIr);
    printk(KERN_INFO "IR Remote: Goodbye from the IR Remote LKM!\n");
}
/*
   Function name: irgpio_irq_handler
Description : Interrupt handler to get unique value for each key when the key is pressed
*/
static irq_handler_t irgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
    static int i,j,input=0;
    getnstimeofday(&ts_current);
    ts_diff = timespec_sub(ts_current, ts_last);            //calculatuing time difference for IRQF_TRIGGER_RISING and IRQF_TRIGGER_FALLING
    ts_last = ts_current;
    isPulse = !isPulse;
    i = (ts_diff.tv_nsec / 1000) / 400;
    if(i<7) {
        if(i==6){
            isPulse = 1;
            nbits = 0;
        }
        for (j=0;j<i;j++){
            rbuf = (rbuf << 1) | isPulse;
        }
        nbits = nbits + i;
        if(nbits >=83){
            data = 0;
            for (i=0; i < 32; i++){
                data = data | (rbuf & (1<<(i*2)));
            }
            printk(KERN_INFO "data = %lx",data); 
            input=unique_val();
            printk(KERN_INFO "IR Remote : input value %d \n",input);
            input_event(button_dev, EV_KEY,input,1);            //Trigering input event when any key is pressed
            input_event(button_dev, EV_KEY,input,0);            //To intimate the release 
            input_sync(button_dev);                             //To intimate that no more data.Intimate event to hal 
            nbits = 0;
        }

    } 
    return (irq_handler_t) IRQ_HANDLED;
}

module_init(irRemote_init);
module_exit(irRemote_exit);
