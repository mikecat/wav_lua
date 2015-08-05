-- パラメータ : 各素子値

function process(data, sample_rate, parameters)
	p = {}
	for i = 1, #parameters do
		p[i] = tonumber(parameters[i])
	end
	result = {}
	if #p <= 1 then
		return data
	elseif #p == 2 then
		for ch = 1, #data do
			result[ch] = {}
			for i = 1, #data[ch] do
				result[ch][i] = data[ch][i] * p[2] / (p[1] + p[2])
			end
		end
	else
		for ch = 1, #data do
			result[ch] = {}
			status = {}
			status_next = {}
			for i = 2, #p - 1 do
				status[i] = 0
			end
			for i = 1, #data[ch] do
				-- 右の電流を求める
				status[#p] = status[#p - 1] / p[#p]
				if #p % 2 == 0 then
					-- 最初がインダクタなので、左の電圧を求める
					status[1] = data[ch][i] - status[2] * p[1]
				else
					-- 最初がキャパシタなので、左の電流を求める
					status[1] = (data[ch][i] - status[2]) / p[1]
				end
				for j = 2, #p - 1 do
					status_next[j] = status[j] + (status[j - 1] - status[j + 1]) / p[j] / sample_rate
				end
				t = status
				status = status_next
				status_next = t
				result[ch][i] = status[#p - 1]
			end
		end
	end
	return result
end
