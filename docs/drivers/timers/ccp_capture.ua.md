[🇬🇧 English version](./ccp_capture.md)

# CCP Capture Драйвер

## Опис
CCP драйвер у режимі Capture для точного захоплення часових міток подій.

## API
- `ccp_capture_init()`
- `ccp_capture_enable()`
- `ccp_capture_disable()`
- `ccp_capture_get_value()`
- `ccp_capture_set_callback()`
- `ccp_capture_irq_handler()`

## Приклад
```c
ccp_capture_init(CCP_CAPTURE_RISING);
ccp_capture_set_callback(on_capture);
ccp_capture_enable();
```

## Особливості
- ISR має викликати `ccp_capture_irq_handler()`.
- Зазвичай використовує Timer1 як базу часу.

## Залежності
- `core/compiler.h`
- `core/types.h`
- База Timer1
