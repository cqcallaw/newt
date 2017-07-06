/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <execution_context.h>
#include <execution_context_list.h>

shared_ptr<ExecutionContextList> ExecutionContextList::From(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContextList> context_parent) {
	if (context->GetLifeTime() == PERSISTENT
			|| context->GetLifeTime() == TEMPORARY) {
		weak_ptr<ExecutionContext> weak = context;
		return shared_ptr<ExecutionContextList>(
				new ExecutionContextList(weak, context_parent));
	} else {
		return shared_ptr<ExecutionContextList>(
				new ExecutionContextList(context, context_parent));
	}

}
