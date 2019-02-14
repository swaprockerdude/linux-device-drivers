#include <linux/init.h>  /*most of the kernel utilities are in init.h*/
#include <linux/module.h> /*utilities related to loadable kernel modules are in module.h*/
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h> /*linux filesystem utilities to be used in this device driver*/

MODULE_LICENSE("GPL"); //to make sure kernel does not crack (taint) 

int count = 1;
module_param(count, int, 0644); //To check the working of paramter being 'count' passed at run-time.


/** Probe function-- its called whenever a device is plugged into the computer. 
		It won't be called if another driver is managing the device
		This means, if you want your driver to manage the device, make sure other 
		driver is unloaded before the device is plugged.
*/
int sim_usb_dd_probe (struct usb_interface *intf, const struct usb_device_id *id)
{
 printk(KERN_ALERT "Inside PROBE func. USB (%04X:%04X) plugged\n",id->idVendor,id->idProduct);
 return 0; //'0'=> this driver will manage the plugged device
}


/** usb_device_id table
		This table stores the IDs of all the USB devices that will be controlled by 
		this driver.
*/
struct usb_device_id sim_usb_dd_table[] = {
	// { USB_DEVICE(vendor_id, product_id) },
	{ USB_DEVICE(0x0781, 0x5567) }, /*obtained from terminal using cmd-- lsusb -v*/
	{}/*terminating entry*/
};


/** MODULE_DEVICE_TABLE enables the linux-hotplug system to load the driver automatically when the 
		device is plugged in. 
*/
MODULE_DEVICE_TABLE(usb, sim_usb_dd_table); 


/** Disconnect func is called when the USB device is unplugged.
*/
void sim_usb_dd_disconnect (struct usb_interface *intf)
{
  printk(KERN_ALERT "Inside DISCONNECT func. USB is disconnected\n");
}


/** struct usb_driver contains all the operations that can be performed on the 
		USB device.  
*/
struct usb_driver sim_usb_dd = {
	.name = "My_USB",
	.id_table = sim_usb_dd_table, /*this contains ids of all the USBs that this driver will serve*/
	.probe = sim_usb_dd_probe,
	.disconnect = sim_usb_dd_disconnect,
};


/** init func to initiate the module to be loaded. Iti called when a USB device 
		is plugged into the system. The double underscore (__) before init implies 
		that the code for init function will be removed from RAM after its call, because 
		it will never be used again. 
*/
__init int sim_usb_dd_init(void)
{
 printk(KERN_ALERT "Inside sim_usb_dd init function\n");
 /*Register the device using following function from -- register_chrdev("Device number", Device name", "file_operations_variable")*/
 /*file_operations_variable is of type struct file_operations in fs.h file. */
 /*it has all the file operations performed on this device*/ 
 usb_register(&sim_usb_dd); //now all the services to the corresponding device will be served by this registered driver
 sim_usb_dd_flag(count);
 return 0;
}


/** exit func to cleanup after the module is unloaded. It is called after the USB 
		device is unplugged from the system. 
*/
void sim_usb_dd_exit(void)
{
  printk(KERN_ALERT "Inside sim_usb_dd exit function\n");
  sim_usb_dd_flag(count);
  usb_deregister(&sim_usb_dd);
}

/** Two macros from init utility to tell the kernel which of the above functions 
*/
module_init(sim_usb_dd_init);
module_exit(sim_usb_dd_exit);



//========================================================================================================================================
/*NOTE:- After inserting module for dd, we have to create a node for it in /dev/.. with the name of this module. Linux treats everything
as a file. To perform functions (I/O operations) of this module after it is loaded, linux will use this node (file).
So, the cmd-> echo "test" > /dev/sim_usb_dd   is a system call for writing "test" into the device  
    the cmd-> cat /dev/sim_usb_dd   is a system call for reading from the device  which are executed only upon creation of this file.
Unloading the module does not delete this file. Reloading the module does necessarily mean creation of this file again, unless it was removed. 

*/


/***********************************************************************************************************************************
Kernal prints when usb_storage driver manages usb on plugging--
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.548766] usb 3-3: new high-speed USB device number 8 using xhci_hcd
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.697367] usb 3-3: New USB device found, idVendor=0781, idProduct=5567
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.697372] usb 3-3: New USB device strings: Mfr=1, Product=2, SerialNumber=3
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.697375] usb 3-3: Product: Cruzer Blade
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.697378] usb 3-3: Manufacturer: SanDisk
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.697381] usb 3-3: SerialNumber: 4C530000160811104293
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.698086] usb-storage 3-3:1.0: USB Mass Storage device detected --->probe fn 
Nov 26 18:06:23 swapnil-Lenovo-G580 kernel: [173782.698405] scsi host6: usb-storage 3-3:1.0
Nov 26 18:06:23 swapnil-Lenovo-G580 mtp-probe: checking bus 3, device 8: "/sys/devices/pci0000:00/0000:00:14.0/usb3/3-3"
Nov 26 18:06:23 swapnil-Lenovo-G580 mtp-probe: bus: 3, device: 8 was not an MTP device
Nov 26 18:06:23 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'bind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-3/3-3:1.0
Nov 26 18:06:23 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'bind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-3
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.717650] scsi 6:0:0:0: Direct-Access     SanDisk  Cruzer Blade     1.00 PQ: 0 ANSI: 6
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.718298] sd 6:0:0:0: Attached scsi generic sg2 type 0
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.718870] sd 6:0:0:0: [sdb] 61341696 512-byte logical blocks: (31.4 GB/29.3 GiB)
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.720044] sd 6:0:0:0: [sdb] Write Protect is off
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.720050] sd 6:0:0:0: [sdb] Mode Sense: 43 00 00 00
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.720297] sd 6:0:0:0: [sdb] Write cache: disabled, read cache: enabled, doesn't support DPO or FUA
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.729846]  sdb: sdb1
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173783.731155] sd 6:0:0:0: [sdb] Attached SCSI removable disk
Nov 26 18:06:24 swapnil-Lenovo-G580 kernel: [173784.184260] FAT-fs (sdb1): Volume was not properly unmounted. Some data may be corrupt. Please run fsck.
Nov 26 18:06:24 swapnil-Lenovo-G580 systemd[1]: Started Clean the /media/swapnil/A317-E542 mount point.
Nov 26 18:06:24 swapnil-Lenovo-G580 dbus-daemon[2190]: [session uid=1000 pid=2190] Activating service name='org.gnome.Shell.HotplugSniffer' requested by ':1.21' (uid=1000 pid=2314 comm="/usr/bin/gnome-shell " label="unconfined")
Nov 26 18:06:24 swapnil-Lenovo-G580 udisksd[708]: Mounted /dev/sdb1 at /media/swapnil/A317-E542 on behalf of uid 1000


Kernal prints when usb_storage driver manages usb on unplugging--
Nov 26 18:09:13 swapnil-Lenovo-G580 kernel: [173953.047481] usb 3-3: USB disconnect, device number 8
Nov 26 18:09:13 swapnil-Lenovo-G580 udisksd[708]: Cleaning up mount point /media/swapnil/A317-E542 (device 8:17 no longer exists)
Nov 26 18:09:13 swapnil-Lenovo-G580 systemd[1]: Stopping Clean the /media/swapnil/A317-E542 mount point...
Nov 26 18:09:13 swapnil-Lenovo-G580 systemd[1]: Stopped Clean the /media/swapnil/A317-E542 mount point.
Nov 26 18:09:13 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'unbind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-3/3-3:1.0
Nov 26 18:09:13 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'unbind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-3
********************************************************************************************************************************************
********************************************************************************************************************************************
Kernal prints when simple_usb_dd driver manages usb on plugging--
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174232.922477] usb 3-4: new high-speed USB device number 9 using xhci_hcd
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.071037] usb 3-4: New USB device found, idVendor=0781, idProduct=5567
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.071042] usb 3-4: New USB device strings: Mfr=1, Product=2, SerialNumber=3
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.071045] usb 3-4: Product: Cruzer Blade
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.071048] usb 3-4: Manufacturer: SanDisk
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.071050] usb 3-4: SerialNumber: 4C530000160811104293
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.071511] Inside PROBE func. USB (0781:5567) plugged --->probe fn
Nov 26 18:13:53 swapnil-Lenovo-G580 mtp-probe: checking bus 3, device 9: "/sys/devices/pci0000:00/0000:00:14.0/usb3/3-4"
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.089387] usbcore: registered new interface driver usb-storage
Nov 26 18:13:53 swapnil-Lenovo-G580 kernel: [174233.091443] usbcore: registered new interface driver uas
Nov 26 18:13:53 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'bind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-4/3-4:1.0
Nov 26 18:13:53 swapnil-Lenovo-G580 mtp-probe: bus: 3, device: 9 was not an MTP device
Nov 26 18:13:53 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'bind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-4

Kernal prints when simple_usb_dd driver manages usb on unplugging--
Nov 26 18:15:36 swapnil-Lenovo-G580 kernel: [174336.330678] usb 3-4: USB disconnect, device number 9
Nov 26 18:15:36 swapnil-Lenovo-G580 kernel: [174336.330774] Inside DISCONNECT func. USB is disconnected
Nov 26 18:15:36 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'unbind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-4/3-4:1.0
Nov 26 18:15:37 swapnil-Lenovo-G580 upowerd[956]: unhandled action 'unbind' on /sys/devices/pci0000:00/0000:00:14.0/usb3/3-4
******************************************************************************************************************************************/











