-- パラメータ : 周波数[Hz](省略時440) 長さ[秒](省略時10) サンプルレート(省略時44100)

function process(data, sample_rate, parameters)
	freq = tonumber(parameters[1] or 440)
	length = tonumber(parameters[2] or 10)
	rate = tonumber(parameters[3] or 44100)
	result = {}
	max = length * rate
	for i = 1, max do
		result[i] = math.sin((i - 1) * math.pi * 2 / (rate / freq))
	end
	return {result}, rate
end
