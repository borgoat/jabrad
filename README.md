## Dependencies

* [GIO](https://developer.gnome.org/gio/stable/)
* [Jabra SDK Linux](https://developer.jabra.com/site/global/sdks/linux/index.gsp)

## Codegen

```shell
gdbus-codegen --generate-c-code jabra-generated --c-namespace gbj --interface-prefix com.github.borgoat.Jabra1. dbus/com.github.borgoat.Jabra1.xml
```
