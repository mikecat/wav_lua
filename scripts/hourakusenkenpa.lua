-- パラメータ : R[Ω](省略時1) C[F](省略時1) ダイオードの電圧降下[V](省略時0)

function process(data, sample_rate, parameters)
	r = tonumber(parameters[1] or 1)
	c = tonumber(parameters[2] or 1)
	offset = tonumber(parameters[3] or 0)
	result = {}
	for ch = 1, #data do
		result[ch] = {}
		denatu = 0
		for i = 1, #data[ch] do
			if data[ch][i] >= denatu + offset then
				denatu = data[ch][i] - offset
			else
				denatu = denatu - ((denatu / r) / sample_rate) / c
			end
			result[ch][i] = denatu
		end
	end
	return result
end
