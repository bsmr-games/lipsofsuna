local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

Widgets.Uitradeitem = Class("Uitradeitem", UiInvItem)

Widgets.Uitradeitem.new = function(clss, item, index, buy)
	local self = UiInvItem.new(clss, nil, item, index, item and item.slot)
	self.buy = buy
	if item then
		self.hint = "$A: Remove item\n$$B\n$$U\n$$D"
	else
		self.hint = "$A: Choose item\n$$B\n$$U\n$$D"
	end
	return self
end

Widgets.Uitradeitem.apply = function(self)
	if self.buy then
		Operators.trading:set_buy_item_index(self.index)
		Operators.trading:add_buy()
	else
		Operators.trading:set_sell_item_index(self.index)
		Operators.trading:add_sell()
	end
	Ui:pop_state()
	Client.effects:play_global("uitransition1")
end
