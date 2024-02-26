local bit = require "bit32"
local cp
local mode = 'cp'

local function printf(fmt, ...)
  io.stdout:write(string.format(fmt, ...))
end

local function parse_line(l)
  local r = 0
  for i=l:len(),1, -1 do
    local c = string.byte(l, i)
    if c == string.byte '-' or c == string.byte ' ' then
      r = r*2
    else
      r = r*2 + 1
    end
  end
  return r
end

local dict = {}
local cur = 0

print "const uint8_t FONT8x10[] PROGMEM ={"
for l in io.lines() do
  if mode == 'cp' then
    if tonumber(l) then
      local nr = tonumber(l)
      if not cp or nr ~= cp + 1 then
--        print(l)
        table.insert(dict, { off = cur, start = nr, range = 1 })
      else
        local t = dict[#dict]
        t.range = t.range + 1
      end
      cp = nr
      printf("\t")
      mode = 'sym'
    end
  elseif mode == 'sym' then
    if l == "" then
      mode = 'cp'
      print()
    else
      printf("0x%02x, ", parse_line(l))
      cur = cur + 1
    end
  end
end
print ""
print "};"
print "const uint16_t FONT8x10_map[] = {"
for _, v in ipairs(dict) do
  printf("\t%d, 0x%04x, %d,\n", v.off, v.start, v.range)
end
print("\t0, 0, 0,")
print "};"

print [[

struct font8 {
	const uint8_t *data;
	const uint16_t *map;
	const int w;
	const int h;
};

const struct font8 font7x10 = {
	.data = FONT8x10,
	.map = FONT8x10_map,
	.w = 8,
	.h = 10,
};
]]
