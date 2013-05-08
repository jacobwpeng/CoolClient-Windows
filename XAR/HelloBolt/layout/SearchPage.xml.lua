local function deepcopy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end  -- if
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end  -- for
        return setmetatable(new_table, getmetatable(object))
    end  -- function _copy
    return _copy(object)
end  -- function deepcopy

function OnInitControl(self)
	self:SetObjPos(0,0,"father.width","father.height")
end

function OnFocusChange(self, isFocus)
	local parent = self:GetParent()
	if isFocus then
		--XLMessageBox("focus")
		parent:SetTextureID("texture.edit.light.bkg")
	else
		parent:SetTextureID("")
	end
end

function OnLogoDown(self)
	local owner = self:GetOwnerControl():GetParent()
	owner:GetControlObject("ctrl"):SetObjPos2(0,0,"father.width","father.height")
	
	local resultPage = owner:GetControlObject("ResultPage")
	if resultPage then
		owner:RemoveChild(resultPage)
	else
		return false
	end
	--将各子控件状态设置为初试
	owner:GetControlObject("inputarea"):SetText("")
end
function OnSearchParamKeyWordsChanged(self)
	local control  = self:GetOwnerControl()
	local attr = control:GetAttribute()
	if attr.NewSearchParam == nil then
		attr.NewSearchParam = {KeyWords, Type = 15}
	end
	attr.NewSearchParam.KeyWords = self:GetText()
end
function OnSearchParamTypeChanged(self, eventName, isCheck)--计算搜索参数的值
	local control  = self:GetOwnerControl()
	local attr = control:GetAttribute()
	if attr.NewSearchParam == nil then
		attr.NewSearchParam = {Type = 15}
	end
	local id = self:GetID()
	local offset
	if id == "checkbox.movie" then
		offset = 1
	end
	if id == "checkbox.music" then
		offset = 2
	end
	if id == "checkbox.game" then
		offset = 4
	end
	if id == "checkbox.book" then
		offset = 8
	end
	if isCheck then
		if attr.NewSearchParam.Type == 15 then
			attr.NewSearchParam.Type = offset
		else
			attr.NewSearchParam.Type = attr.NewSearchParam.Type + offset
		end
	else
		attr.NewSearchParam.Type = attr.NewSearchParam.Type - offset
	end
	
	if attr.NewSearchParam.Type == 0 then
		attr.NewSearchParam.Type = 15
	end
	--XLMessageBox(attr.NewSearchParam.Type.." text:"..attr.NewSearchParam.KeyWords)
end 

function OnSearchPageSearchBtnClick(self)--搜索页面的按钮响应
	local ctrl = self:GetOwnerControl()
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local resultPage = objFactory:CreateUIObject("ResultPage", "ResultPage")
	local listbox = resultPage:GetControlObject("listbox")
	
	local attr = resultPage:GetAttribute()
	local edit = ctrl:GetControlObject("inputarea")
	local movie = ctrl:GetControlObject("checkbox.movie")
	local music = ctrl:GetControlObject("checkbox.music")
	local game = ctrl:GetControlObject("checkbox.game")
	local book = ctrl:GetControlObject("checkbox.book")
	if attr.SearchParam == nil then
		attr.SearchParam = {}
	end
	attr.SearchParam.Type = 0
	if movie:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 1
	end
	if music:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 2
	end
	if game:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 4
	end
	if book:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 8
	end
	if attr.SearchParam.Type == 0 then
		attr.SearchParam.Type = 15
	end
	attr.SearchParam.KeyWords = edit:GetText()
	attr.CurPage = 1
	
	
	--将搜索页面的UI移出
	ctrl:GetControlObject("ctrl"):SetObjPos2(1920,1080,"father.width","father.height")	
	resultPage:SetObjPos2(0,0,"father.width","father.height")
	ctrl:AddChild(resultPage)
	
	local tab = {}
	--[[这里填你的函数
	
	tab.Name = "King of Fight"
	]]
	listbox:AddItem(tab)
	listbox:UpdateUI()
end

function OnResultPageSearchBtnClick(self)--结果页面的搜索按钮
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local edit = ctrl:GetControlObject("inputarea")
	local movie = ctrl:GetControlObject("checkbox.movie")
	local music = ctrl:GetControlObject("checkbox.music")
	local game = ctrl:GetControlObject("checkbox.game")
	local book = ctrl:GetControlObject("checkbox.book")
	local listbox = ctrl:GetControlObject("listbox")
	if attr.SearchParam == nil then
		attr.SearchParam = {}
	end
	attr.SearchParam.Type = 0
	if movie:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 1
	end
	if music:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 2
	end
	if game:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 4
	end
	if book:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 8
	end
	if attr.SearchParam.Type == 0 then
		attr.SearchParam.Type = 15
	end
	attr.SearchParam.KeyWords = edit:GetText()
	attr.CurPage = 1
	--更新listbox数据
end


function OnFirstPageClick(self)--回到结果首页
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	if attr.CurPage == 1 then
		return
	end
	
	listbox:ResetContent()
	--拿数据
	local tab = {}
	listbox:AddItem(tab)
	listbox:UpdateUI()
	attr.CurPage = 1
end

function OnPrePageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	if attr.CurPage == 1 then
		return
	end
	
	listbox:ResetContent()
	--拿数据
	local tab = {}
	listbox:AddItem(tab)
	listbox:UpdateUI()
	attr.CurPage = attr.CurPage - 1
end

function OnNextPageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	if attr.CurPage == 1 then
		return
	end
	
	listbox:ResetContent()
	--拿数据
	local tab = {}
	listbox:AddItem(tab)
	listbox:UpdateUI()
	attr.CurPage = attr.CurPage + 1
end

function OnLastPageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	if attr.CurPage == 1 then
		return
	end
	
	listbox:ResetContent()
	--拿数据
	local tab = {}
	listbox:AddItem(tab)
	listbox:UpdateUI()
	--attr.CurPage = attr.CurPage 
end

function OnSkipPageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	local desPage = ctrl:GetControlObject("skippage.input")
	if desPage:GetText() == attr.CurPage then
		return
	end
	
	listbox:ResetContent()
	--拿数据
	local tab = {}
	listbox:AddItem(tab)
	listbox:UpdateUI()
	attr.CurPage = desPage:GetText()
end