# Memfault Network Example

Example application for ModusToolbox, loosely based on the
[mtb-example-anycloud-secure-tcp-client ](https://github.com/Infineon/mtb-example-anycloud-secure-tcp-client) application.

This code example demonstrates the integration of Memfault with PSoC&trade; 6 MCU with AIROC™ CYW43xxx Wi-Fi & Bluetooth® combo chips.

For step by steps on how to integrate the
[memfault-firmware-sdk](https://github.com/memfault/memfault-firmware-sdk/commits) into your own
project, see the following guide: https://mflt.io/mtb-integration-guide

This example application was tested with the following setup:

- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; pioneer kit](https://www.cypress.com/CY8CKIT-062S2-43012)
- (`CY8CKIT-062S2-43012`) using GNU Arm® embedded compiler v10.3.1 (`GCC_ARM`)

## Requirements

- [ModusToolbox&trade; software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/) v3.0 or later (tested with v3.0)
- PSoC&trade; 6 Board support package (BSP) minimum required version: 4.0.0
- Programming language: C
- Associated parts: All [PSoC&trade; 6 MCU](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-6-32-bit-arm-cortex-m4-mcu)

## Hardware setup

This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.

**Note:** The PSoC&trade; 6 Bluetooth&reg; LE pioneer kit (CY8CKIT-062-BLE) and the PSoC&trade; 6 Wi-Fi Bluetooth&reg; pioneer kit (CY8CKIT-062-WIFI-BT) ship with KitProg2 installed. The ModusToolbox&trade; software requires KitProg3. Before using this code example, make sure that the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/Infineon/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

## Software setup

Install a terminal emulator if you don't have one such as
[minterm](https://github.com/pyserial/pyserial/blob/master/serial/tools/miniterm.py) or [Tera Term](https://ttssh2.osdn.jp/index.html.en).

Install a Python interpreter if you don't have one. This code example is tested using [Python 3.7.7](https://www.python.org/downloads/release/python-377/).

Install the Python package `mflt_build_id` in your Python system packages.

## Using the code example

## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. The kit is configured to run in Wi-Fi STA interface mode.

   1. Modify the `WIFI_SSID`, `WIFI_PASSWORD` macros to match that of the Wi-Fi network credentials
      that you want to connect to. These macros are defined in the
      [source/memfault_example_app.h](source/memfault_example_app.h) file.
   2. (Optional) If compile-time configuration is not available, the commands `wifi_scan`, `wifi_join`,
      `wifi_save` are provided to scan for networks, join a found network, and save a network for
      auto-conneciton at boot.

3. If you already have a Memfault account, navigate [here](https://mflt.io/project-key) to create a
   project key. (If you do not have an account sign up for a Memfault account
   [here](https://mflt.io/signup) and create a project). Modify the `MEMFAULT_PROJECT_KEY` macro in
   [source/memfault_example_app.h](source/memfault_example_app.h) file.

4. Setup up the application using one of the following:

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade; software</b></summary>

   1. Select New Application from the Quick Panel
   2. Select your board
   3. Select Memfault Network Example from the Wi-Fi section
   4. After the application is created, with the application selected in the projects panel, click Build Application from the Quick Panel

   <details><summary><b>Using CLI</b></summary>

   1. Clone the application to your workspace.
   2. Navigate into the application root and run:

   ```bash
   <PATH_TO_MODUS_TOOLS>/library-manager/library-manager-cli --project . --add-bsp-name <BSP_NAME> --add-bsp-version <BSP_VERSION> --add-bsp-location "local"
   <PATH_TO_MODUS_TOOLS>/library-manager/library-manager-cli --project . --set-active-bsp APP_<BSP_NAME>
   make build
   ```

5. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.

6. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade; software</b></summary>

   1. Select the application project in the Project Explorer.

   2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**.
   </details>

   <details><summary><b>Using CLI</b></summary>

   From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain and target are specified in the application's Makefile but you can override those values manually:

   ```
   make program TARGET=<BSP> TOOLCHAIN=<toolchain>
   ```

   Example:

   ```
   make program TARGET=CY8CPROTO-062-4343W TOOLCHAIN=GCC_ARM
   ```

      </details>

7. Upload the symbol file to see decoded data on the Memfault web app (upload the .elf file, either via the web app or using the [Memfault CLI tool](https://docs.memfault.com/docs/mcu/symbol-file-build-ids/#upload-symbol-files-cli))

## Running the Example Application

The board will automatically attempt to bring up the network interface on boot. If it fails,
posting data from the board will not work, but the CLI will still be available for manually
uploading chunk data. On success you should see something like the following:

```
[I] Memfault Build ID: 1d63b5daaa91c74ad352d16533417604219beec1
[I] S/N: DEMOSERIAL
[I] SW type: app-fw
[I] SW version: 1.0.0-dev
[I] HW version: dvt1
[I] Reset Reason, Cy_SysLib_GetResetReason=0x10
[I] Reset Cause:
[I]  Software Reset
[I] Memfault Initialized!

WLAN MAC Address : C4:AC:59:9C:5C:5C
WLAN Firmware    : wl0: Apr 12 2022 20:39:36 version 13.10.271.287 (760d561 CY) FWID 01-b438e2a0
WLAN CLM         : API: 18.2 Data: 9.10.0 Compiler: 1.36.1 ClmImport: 1.34.1 Creation: 2021-04-26 04:01:15
WHD VERSION      : v2.3.0 : v2.3.0 : GCC 10.3 : 2022-04-13 14:02:24 +0800
[I] Wi-Fi Connection Manager initialized.
[I] Successfully connected to Wi-Fi network
[I] IPv4 address assigned: 192.168.1.19

[I] Secure Socket initialized
[I] Global trusted RootCA certificate loaded
[I] Posting Memfault Data...
[D] Response Complete: Parse Status 0 HTTP Status 202!
[D] Body: Accepted
```

To see debug options, run the `help` command:

```
mflt> help
get_core: Get coredump info
clear_core: Clear an existing coredump
crash: Trigger a crash
trigger_logs: Trigger capture of current log buffer contents
drain_chunks: Flushes queued Memfault data. To upload data see https://mflt.io/posting-chunks-with-gdb
trace: Capture an example trace event
get_device_info: Get device info
reboot: Reboot system and tracks it with a trace event
export: Export base64-encoded chunks. To upload data see https://mflt.io/chunk-data-export
help: Lists all commands
```

An example crash can be forced using the `crash` command:

```
mflt> crash 1
[... device will reboot and post data on reboot ...]
[I] Memfault Build ID: 5f455f3ce017f5f93666dfbc6fa016b680b72901
[I] S/N: DEMOSERIAL
[I] SW type: app-fw
[I] SW version: 1.0.0-dev
[I] HW version: dvt1
[I] Reset Reason, Cy_SysLib_GetResetReason=0x10
[I] Reset Cause:
[I]  Software Reset
[I] Memfault Initialized!

mflt> WLAN MAC Address : C4:AC:59:9C:5C:5C
WLAN Firmware    : wl0: Apr 12 2022 20:39:36 version 13.10.271.287 (760d561 CY) FWID 01-b438e2a0
WLAN CLM         : API: 18.2 Data: 9.10.0 Compiler: 1.36.1 ClmImport: 1.34.1 Creation: 2021-04-26 04:01:15
WHD VERSION      : v2.3.0 : v2.3.0 : GCC 10.3 : 2022-04-13 14:02:24 +0800
[I] Wi-Fi Connection Manager initialized.
[I] Successfully connected to Wi-Fi network 'MyCharterWiFif2-2G'
[I] IPv4 address assigned: 192.168.1.19

[I] Secure Socket initialized
[I] Global trusted RootCA certificate loaded
[D] Response Complete: Parse Status 0 HTTP Status 202!
[D] Body: Accepted
[D] Response Complete: Parse Status 0 HTTP Status 202!
[D] Body: Accepted
```

A 1-second timer runs that will attempt to upload any data generated by the Memfault library (core
dumps, metrics, etc). For more information about how to use the demo CLI, refer to https://mflt.io/demo-cli

## Debugging

You can debug the example to step through the code. In the IDE, use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For details, see the "Program and debug" section in the [Eclipse IDE for ModusToolbox&trade; software user guide](https://www.cypress.com/MTBEclipseIDEUserGuide).

**Note:** **(Only while debugging)** On the CM4 CPU, some code in `main()` may execute before the debugger halts at the beginning of `main()`. This means that some code executes twice – once before the debugger stops execution, and again after the debugger resets the program counter to the beginning of `main()`. See [KBA231071](https://community.cypress.com/docs/DOC-21143) to learn about this and for the workaround.

## Design and implementation

### Resources and settings

**Table 1. Application resources**

| Resource   | Alias/object            | Purpose                                                 |
| :--------- | :---------------------- | :------------------------------------------------------ |
| SDIO (HAL) | sdio_obj                | SDIO interface for Wi-Fi connectivity                   |
| UART (HAL) | cy_retarget_io_uart_obj | UART HAL object used by retarget-io for debug UART port |
| LED (BSP)  | CYBSP_USER_LED          | User LED to show output                                 |

<br>

This example uses the Arm® Cortex®-M4 (CM4) CPU of PSoC&trade; 6 MCU to execute an RTOS task (TCP client task). At device reset, the default Cortex®-M0+ (CM0+) application enables the CM4 CPU and configures the CM0+ CPU to go to sleep.

In this example, the TCP client establishes a secure connection with a TCP server through SSL handshake. During the SSL handshake, the client presents its SSL certificate (self-signed) for verification and also verifies the server's identity to which it is connecting. Once the SSL handshake completes successfully, the TCP client controls the user LED ON or OFF based on the command received from the TCP server.

<br>

## Related resources


Resources  | Links
-----------|----------------------------------
Application notes  | [AN228571](https://www.infineon.com/dgdl/Infineon-AN228571_Getting_started_with_PSoC_6_MCU_on_ModusToolbox_software-ApplicationNotes-v06_00-EN.pdf?fileId=8ac78c8c7cdc391c017d0d36de1f66d1&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-an_vanitylink) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; software <br>  [AN215656](https://www.infineon.com/dgdl/Infineon-AN215656_PSoC_6_MCU_Dual-CPU_System_Design-ApplicationNotes-v09_00-EN.pdf?fileId=8ac78c8c7cdc391c017d0d3180c4655f&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-an_vanitylink) – PSoC&trade; 6 MCU: Dual-CPU system design | [Using ModusToolbox&trade; software](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub <br> [Using PSoC&trade; Creator](https://www.infineon.com/cms/en/design-support/software/code-examples/psoc-3-4-5-code-examples-for-psoc-creator/?utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-code_example)
Device documentation | [PSoC&trade; 6 MCU datasheets](https://www.infineon.com/cms/en/search.html#!view=downloads&term=psoc6&doc_group=Data%20Sheet) <br> [PSoC&trade; 6 technical reference manuals](https://www.infineon.com/cms/en/search.html#!view=downloads&term=psoc6&doc_group=Additional%20Technical%20Information)
Development kits | Select your kits from the [Evaluation board finder](https://www.infineon.com/cms/en/design-support/finder-selection-tools/product-finder/evaluation-board)
Libraries on GitHub  | [mtb-pdl-cat1](https://github.com/Infineon/mtb-pdl-cat1) – PSoC&trade; 6 peripheral driver library (PDL)  <br> [mtb-hal-cat1](https://github.com/Infineon/mtb-hal-cat1) – Hardware abstraction layer (HAL) library <br> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port
Middleware on GitHub  | [capsense](https://github.com/Infineon/capsense) – CAPSENSE&trade; library and documents <br> [psoc6-middleware](https://github.com/Infineon/modustoolbox-software#psoc-6-middleware-libraries) – Links to all PSoC&trade; 6 MCU middleware
Tools  | [Eclipse IDE for ModusToolbox&trade; software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/) – ModusToolbox&trade; software is a collection of easy-to-use software and tools enabling rapid development with Infineon MCUs, covering applications from embedded sense and control to wireless and cloud-connected systems using AIROC&trade; Wi-Fi and Bluetooth® connectivity devices.

<br>

## Other resources

Infineon provides a wealth of data at www.infineon.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC&trade; 6 MCU devices, see [How to design with PSoC&trade; 6 MCU – KBA223067](https://community.infineon.com/t5/Knowledge-Base-Articles/How-to-Design-with-PSoC-6-MCU-KBA223067/ta-p/248857) in the Infineon community.

<br>
---------------------------------------------------------

© Cypress Semiconductor Corporation, 2020-2022. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, WICED, ModusToolbox, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit cypress.com. Other names and brands may be claimed as property of their respective owners.
