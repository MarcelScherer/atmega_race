# atmega_race

<p align="center">
  <img src="https://github.com/MarcelScherer/atmega_race/blob/master/Time_measurement_arduino/doku/bild.jpg" width="700" title="Bild">
</p>


The atmega race project built a box with which you can measure lap times. The concept is, that you interrupt the light signal between a strong flashlight and the lighting sensor in the tube.
You need :
- [Arduino](https://www.aliexpress.com/item/1pcs-lot-Nano-Atmega168-controller-compatible-for-arduino-nano-Atmega168P-CH340-CH340C-replace-CH340G-USB-driver/32860026559.html?spm=2114.search0104.3.8.fe532b20x9cTup&ws_ab_test=searchweb0_0,searchweb201602_4_10320_5017015_10065_10068_10843_10547_5017315_10059_10548_10696_100031_10319_10084_5017115_10083_10103_451_452_10618_10304_10307_10820_10821_10302_5017215,searchweb201603_45,ppcSwitch_5&algo_expid=6b95ccd8-77ab-472a-9718-3bd8b8e6435d-1&algo_pvid=6b95ccd8-77ab-472a-9718-3bd8b8e6435d&priceBeautifyAB=0)
- [Lightingsensor](https://www.aliexpress.com/item/Free-shipping-Photodiode-module-detects-brightness-light-sensitive-light-detector-module-smart-car-for-arduino/32414962058.html?spm=2114.search0104.3.8.25f476c4S1WK7v&ws_ab_test=searchweb0_0,searchweb201602_4_10320_5017015_10065_10068_10843_10547_5017315_10059_10548_10696_100031_10319_10084_5017115_10083_10103_451_452_10618_10304_10307_10820_10821_10302_5017215,searchweb201603_45,ppcSwitch_5&algo_expid=84601aab-92f9-4933-9a55-71bff8b0584b-1&algo_pvid=84601aab-92f9-4933-9a55-71bff8b0584b&priceBeautifyAB=0)
- [LCD-Display](https://www.aliexpress.com/item/1PCS-LCD1602-1602-module-green-screen-16x2-Character-LCD-Display-Module-1602-5V-green-screen-and/32511014601.html?spm=2114.search0104.3.37.f4364b3cVcAwBk&ws_ab_test=searchweb0_0,searchweb201602_4_10320_5017015_10065_10068_10843_10547_5017315_10059_10548_10696_100031_10319_10084_5017115_10083_10103_451_452_10618_10304_10307_10820_10821_10302_5017215,searchweb201603_45,ppcSwitch_5&algo_expid=54dd55f6-1c94-429f-9c2c-477426bb86e4-5&algo_pvid=54dd55f6-1c94-429f-9c2c-477426bb86e4&priceBeautifyAB=0) (without I²C)

Description: The blue led is on if the lighting signal is interrupted. If the green led is on, the time measurement is ready to start the lap. If you interrupt the light signal than you start the measurement, and the orange led goes on. By the next interrupt (min 2 sec. after start) the measurement stops and the red led goes on.
After 5 seconds, after the laps finished, the state goes back to READY and the green led goes on.
The first row of the Display shows the actual time measurement. The second row shows the last time or the fast time, depend on the choice switch.


After you have compiled flashed the [Sorce](https://github.com/MarcelScherer/atmega_race/blob/master/Time_measurement_arduino/time_measurment/time_measurment.ino) to the arduino nano, you have connect the components in the way:

<p align="center">
  <img src="https://github.com/MarcelScherer/atmega_race/blob/master/Time_measurement_arduino/doku/aufbau.PNG" title="aufbau">
</p>

