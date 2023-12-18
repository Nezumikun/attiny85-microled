# attiny85-microled

Прошивка для Attiny85.
Гирлянда из 100 светодиодов. 8 эффектов с вариациями. Переключение между режимами по кнопке + деморежим.
Деморежим: показывает все эффекты по порядку, затем переключает их в случайной последовательности.
Деморежим запускается при включении.
Выход из деморежима: долгое нажатие на кнопку.
Возврат в деморежим: долгое нажатие на кнопку.
Короткое нажатие на кнопку:
* в демо-режиме: включает или выключает гирлянду
* не в демо-режиме: переход к следующему эффекту

Проект для Visual Studio Code + PlatformIO

Проект платы, для которой написана прошивка, можно посмотреть [здесь](https://oshwlab.com/nezumikun/happy-new-year-lights_copy)

Использованы библиотеки:
* [microLED от AlexGyver](https://github.com/GyverLibs/microLED)
* [FastLED](https://github.com/FastLED/FastLED)
