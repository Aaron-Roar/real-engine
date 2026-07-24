# Error Handling {#errors}

Fallible engine functions return explicit result types generated with
`ERROR_DECLARE_RESULT_TYPE`.

Use `error_check(result)` to test whether a result contains an error:

```c
EngineResult result = engine_init();
if(error_check(result)) {
    console_write(LOG_ENGINE, error_default_message(result.result.error));
}
```

Engine code should return real errors to the application instead of printing
and returning early. Applications and examples can decide whether to print the
default message from `error_default_message()` or provide their own message.
