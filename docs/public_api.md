# Public API Reference

The public application-facing API lives in:

- [include/rohr.h](../include/rohr.h)

Application code should include only this facade when possible:

```c
#include "rohr.h"
```

Every public function uses the `rohr_` prefix and is documented in the header
with Doxygen comments covering its purpose, parameters, return value, and error
result behavior where applicable.

For the full generated API reference with searchable function pages, run:

```sh
make docs
```

Then open:

```text
build/docs/html/rohr_8h.html
```

The generated Doxygen output is local build output and is not committed to the
repo.
