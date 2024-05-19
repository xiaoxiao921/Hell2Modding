# Class: rom.config.config_file

A helper class to handle persistent data.

## Constructors (1)

### `new(config_path, save_on_init)`

Create a new config file at the specified config path.

- **Parameters:**
  - `config_path` (string): Full path to a file that contains settings. The file will be created as needed.
  - `save_on_init` (bool): If the config file/directory doesn't exist, create it immediately.

**Example Usage:**
```lua
myInstance = config.config_file:new(config_path, save_on_init)
```

## Functions (5)

### `bind(section, key, default_value, description)`

Create a new setting. The setting is saved to drive and loaded automatically.
Each section and key pair can be used to add only one setting,
trying to add a second setting will throw an exception.

- **Parameters:**
  - `section` (string): Section/category/group of the setting. Settings are grouped by this.
  - `key` (string): Name of the setting.
  - `default_value` (bool or number or string): Value of the setting if the setting was not created yet.
  - `description` (string): Simple description of the setting shown to the user.

- **Returns:**
  - `config_entry`: new config_entry object.

**Example Usage:**
```lua
config_entry = rom.config.config_file:bind(section, key, default_value, description)
```

### `save()`

Writes the config to disk.

**Example Usage:**
```lua
rom.config.config_file:save()
```

### `reload()`

Reloads the config from disk. Unsaved changes are lost.

**Example Usage:**
```lua
rom.config.config_file:reload()
```

### `get()`

- **Returns:**
  - `val`: bool or double or string. Value of this setting

**Example Usage:**
```lua
val = rom.config.config_file:get()
```

### `set(new_value)`

- **Parameters:**
  - `new_value` (bool or double or string): New value of this setting.

**Example Usage:**
```lua
rom.config.config_file:set(new_value)
```

