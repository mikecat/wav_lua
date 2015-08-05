-- パラメータ : 倍率(省略時自動)

function process(data, sample_rate, parameters)
	if #parameters < 1 then
		auto_bairitu = 1
		bairitu = 0
	else
		auto_bairitu = 0
		bairitu = parameters[1]
	end
	result = {}
	for c = 1, #data do
		result[c] = {}
		if auto_bairitu then
			max = 0
			for i = 1, #data[c] do
				v = math.abs(data[c][i])
				if v > max then
					max = v
				end
			end
			if max > 0 then
				bairitu = 1 / max
			else
				bairitu = 1
			end
		end
		for i = 1, #data[c] do
			result[c][i] = bairitu * data[c][i]
		end
	end
	return result
end
