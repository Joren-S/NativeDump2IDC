# NativeDump2IDC
usage:
- Make sure HashMap.h is updated. (see 1.)
- Make sure natives.json is updated. (see 2.)
- Make sure natives_dump is updated. (see 3.)

Using the outputted/created IDC script file:

![usage](http://i.imgur.com/ZY50PrF.png)

Or just press ALT + F7.

- - - -

#### 1.

Make sure the array is formatted like this:

```
uint64_t HASH_MAP[] = {
	ORIGINAL_HASH, ROTATED_HASH,  
	ORIGINAL_HASH, ROTATED_HASH,
  ...
}
```


#### 2. 

The latest natives.json file can be downloaded at [dev-c.com](http://www.dev-c.com/nativedb/)

Note: some DLC natives are missing from this one.


#### 3. 

Make sure the dump is formatted like this:

HASH : Address
HASH : Address
HASH : Address
...

The spaces before and after the ':' are important aswell!
