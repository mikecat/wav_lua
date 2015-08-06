-- パラメータ : オフセット[ミリ秒] 倍率(省略時1) の繰り返し(倍率は最後以外省略不可)

function process(data, sample_rate, parameters)
	offsets = {0}
	bairitu = {1}
	for i = 1, #parameters, 2 do
		offsets[i // 2 + 2] = tonumber(parameters[i]) / 1000 * sample_rate
		if i + 1 <= #parameters then
			bairitu[i // 2 + 2] = tonumber(parameters[i + 1])
		else
			bairitu[i // 2 + 2] = 1
		end
	end
	result = {}
	for c = 1, #data do
		result[c] = {}
		for i = 1, #data[c] do
			for j = 1, #offsets do
				if type(result[c][i + offsets[j]]) == "nil" then
					result[c][i + offsets[j]] = 0
				end
				result[c][i + offsets[j]] = result[c][i + offsets[j]] + bairitu[j] * data[c][i]
			end
		end
	end
	return result
end
